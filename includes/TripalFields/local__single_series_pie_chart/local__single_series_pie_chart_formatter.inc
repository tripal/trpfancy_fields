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
class local__single_series_pie_chart_formatter extends ChadoChartFormatter {

  // The default lable for this field.
  public static $default_label = 'Single-Series Pie Chart';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__single_series_chart');

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
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

    // Grab the name of the field to create a unique ID for the chart.
    $field_name = $this->instance['field_name'];

    // Get the settings
    $settings = $display['settings'];
    // Fill in defaults for any settings not set.
    foreach (self::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    if (!empty($items[0]['value'])) {

      // Process items to expose to JS.
      $data = array();
      foreach ($items as $key => $value) {
        foreach ($value['value']['hydra:member'] as $part) {
          $data[] = array(
            'label' => $part['rdfs:label'],
            'count' => $part['local:count'],
          );
        }
      }

      // Expose the data & settings to JS.
      drupal_add_js(array('trpfancyFields' => array($field_name => $data)), 'setting');

      // Load the Tripal D3 API
      tripald3_load_libraries();

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

      // Finally, add the markup.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => '
          <div id="trpfancy-'.$field_name.'">
            <!-- JS should draw the pie chart here -->
          </div>',
      );

    }
  }

}
