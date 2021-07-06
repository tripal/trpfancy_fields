<?php
/**
 * @class
 * Purpose: Provide a graphical summary of data stored in a materialized view.
 *   This is a generic, configurable fields to make it easier to add charts
 *   to Tripal Content pages.
 *
 * Display:
 *   Draws a pie chart using D3.js
 * Configuration:
 *   None yet available.
 */
class ChadoChartFormatter extends ChadoFieldFormatter {

  // The default lable for this field.
  public static $default_label = 'Chado Chart';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = [];

  // The list of default settings for this formatter.
  // Due to the different charts available and the dependance upon the data,
  // it is too difficult to make a good default for the title and legend.
  // As such I am making a horrible one in the hopes of making admin change it ;-p.
  public static $default_settings = [
    'title' => 'Fancy Chart summarizing Stuff',
    'legend' => 'This should describe the chart including the data that goes into it and how to interpret that data. Furthermore, it is always a good idea to highlight how users can interact with this chart.',
  ];

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * param $field
   *   The field structure being configured.
   * param $instance
   *   The instance structure being configured.
   * param $view_mode
   *   The view mode being configured.
   * param $form
   *   The (entire) configuration form array, which will usually have no use
   *   here.  Typically for reference only.
   * param $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function settingsForm($view_mode, $form, &$form_state) {
    $element = [];

    // Retrieve the current settings.
    $settings = $this->instance['display'][$view_mode]['settings'];
    if (!isset($settings['title'])) { $settings['title'] = self::$default_settings['title']; }
    if (!isset($settings['legend'])) { $settings['legend'] = self::$default_settings['legend']; }

    // This has caught me a couple of times so I'm going to remind both myself and others ;-p.
    $element['msg'] = [
      '#type' => 'markup',
      '#markup' => '<div class="messages warning">Settings are only saved after you click "Update" within this box <br />AND "Save" at the bottom of the page!</div>',
    ];

    // Add in all the general settings
    // which can be passed to TripalD3.drawFigure().
    //-------------------------------------------
    $element['title'] = [
      '#type' => 'textfield',
      '#title' => 'Figure Title',
      '#default_value' => $settings['title'],
      '#description' => 'A breif title for the figure.',
    ];

    $element['legend'] = [
      '#type' => 'textarea',
      '#title' => 'Figure Legend',
      '#default_value' => $settings['legend'],
      '#description' => 'Should describe the figure to the user.',
    ];

    return $element;
  }

  /**
   *  Provides the display for a field
   *
   *  This function corresponds to the hook_field_formatter_view()
   *  function of the Drupal Field API.
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   *  @param $element
   *  @param $entity_type
   *  @param $entity
   *  @param $langcode
   *  @param $items
   *  @param $display
   *
   *  @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // THIS SHOULD BE DONE BY THE CHILD CLASSES
    // SINCE IT'S SPECIFIC TO THE CHART BEING DISPLAYED.
    return;

    // AS A GUIDE:
    // 1. Grab the field name & current settings.
    $field_name = $this->instance['field_name'];

    $settings = $display['settings'];
    foreach (self::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    // 2. Transform the data in $items to the format required by your chart
    //    and make it available to the chart via drupal_add_js as "settings".
    if (!empty($items[0]['value'])) {
      $data = [];
      foreach ($items as $key => $value) {
        // Add it to the data array.
      }
      drupal_add_js(array('trpfancyFields' => array($field_name => $data)), 'setting');

      // 3. Load Tripal D3 API
      tripald3_load_libraries();

      // 4. Use the Tripal D3 API to draw your chart.
      //    If you are not using the API but instead have your own custom chart
      //    then use drupal_add_js to add a file here rather then embedding
      //    the JS inline.
      drupal_add_js('
        Drupal.behaviors.'.$field_name.' = {
          attach: function (context, settings) {

            // Pull the data out of the javascript settings.
            var data = Drupal.settings.trpfancyFields.'.$field_name.';

            // Draw your chart.
            tripalD3.drawFigure(
              data,
              {
                "chartType" : "simplepie",
                "elementId": "trpfancy-'.$field_name.'",
                "height": 250,
                "width": 500,
                "keyPosition": "right",
                "title": "' . $settings['title'] . '",
                "legend": "' . $settings['legend'] . '",
              }
            );
          }
        };',
        array('type' => 'inline')
      );

      // 5. Add the HTML element for the JS to attach the chart to.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => '
          <div id="trpfancy-'.$field_name.'">
            <!-- JS should draw the pie chart here -->
          </div>',
      );

    }
  }

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function settingsSummary($view_mode) {

    // IF YOUR FORMATTER PROVIDES MORE THEN THE DEFAULT OPTIONS,
    // YOU SHOULD OVERRIDE THIS TO SHOW THE VALUES OF THOSE SETTINGS.
    $settings = $this->instance['display'][$view_mode]['settings'];
    return '<strong>Title:</strong> ' . $settings['title'];
  }
}
