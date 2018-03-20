<?php
/**
 * @class
 * Purpose: Provide a quick search on entity pages which submits/redirects to a full search.
 *
 * Display: A simple textfield search form.
 * Configuration:
 *   - path to the full search.
 *   - the URL token (query parameter) the value applies to.
 *   - help text.
 *   - textfield placeholder.
 *   - search button text.
 *   - autocomplete path.
 */
class local__quick_search_formatter extends TripalFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Simple Quick Search';
  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__quick_search');
  // The list of default settings for this formatter.
  public static $default_settings = array(
    'path' => '',
    'key' => 'value',
    'help_text' => 'Enter your search terms below. On submit, a new page will be opened where additional search criteria are available.',
    'placeholder' => 'Search terms',
    'button_text' => 'Search',
    'autocomplete_path' => '',
  );

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

    // Grab the parent settings form in all it's glory.
    $element = parent::settingsForm($view_mode, $form, $form_state);

    // Retrieve the current settings not handled by our parent.
    $settings = $this->instance['display'][$view_mode]['settings'];
    $our_setting_keys = array('path', 'key', 'help_text', 'placeholder', 'button_text', 'autocomplete_path');
    foreach ($our_setting_keys as $k) {
      if (!isset($settings[$k])) { $settings[$k] = self::$default_settings[$k]; }
    }

    // This has caught me a couple of times so I'm going to remind both myself and others ;-p.
    $element['msg'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="messages warning">Settings are only saved after you click "Update" within this box <br />AND "Save" at the bottom of the page!</div>',
    );

    $element['path'] = array(
      '#type' => 'textfield',
      '#title' => 'Search Path',
      '#required' => TRUE,
      '#description' => 'This is the relative path of the search page you would like to redirect the user to.',
      '#default_value' => $settings['path'],
    );

    $element['key'] = array(
      '#type' => 'textfield',
      '#title' => 'Query Parameter Key',
      '#required' => TRUE,
      '#description' => 'This key is used in the query parameters to tell your full search which filter to apply the value to.',
      '#default_value' => $settings['key'],
    );

    $element['button_text'] = array(
      '#type' => 'textfield',
      '#title' => 'Button Text',
      '#required' => TRUE,
      '#description' => 'The text on the search button.',
      '#default_value' => $settings['button_text'],
    );

    $element['placeholder'] = array(
      '#type' => 'textfield',
      '#title' => 'Placeholder Text',
      '#description' => 'This will appear greyed out within the search textfield and should be used to tell your user the kinds of values they can enter.',
      '#default_value' => $settings['placeholder'],
    );

    $element['autocomplete_path'] = array(
      '#type' => 'textfield',
      '#title' => 'Autocomplete Path',
      '#description' => 'The path the AJAX autocomplete script uses as the source for autocompletion.',
      '#default_value' => $settings['autocomplete_path'],
    );

    $element['help_text'] = array(
      '#type' => 'textarea',
      '#title' => 'Help Text',
      '#description' => 'Guidance to your users for how to use the quick search form.',
      '#default_value' => $settings['help_text'],
    );

    return $element;
  }

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
    // Get the settings
    $settings = $display['settings'];

    // Retrieve my form (implemented below this class).
    $form = drupal_get_form('local__quick_search_field_view_form', $settings);

    // Render it and pass it as the markup for this field.
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => drupal_render($form),
    );

    return $element;
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

    $settings = $this->instance['display'][$view_mode]['settings'];
    return '<strong>Full Search Path:</strong> ' . $settings['path'] . '?' . $settings['key'] . '=[search terms]';

  }

}

/**
 * Quick Search form rendered via local__quick_search::view on Tripal Content Pages.
 *
 * @param $form
 *   Nested array of form elements that comprise the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 *
 * @return
 *   Completed $form above.
 */
function local__quick_search_field_view_form($form, $form_state) {

  // Retrieve the formatter settings from thr build info.
  $settings = $form_state['build_info']['args'][0];

  $form['path'] = array(
    '#type' => 'hidden',
    '#value' => $settings['path'],
  );

  $form['key'] = array(
    '#type' => 'hidden',
    '#value' => $settings['key'],
  );

  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => 'Search Terms',
    '#title_display' => 'invisible',
    '#attributes' => array(
      'placeholder' => $settings['placeholder'],
    ),
    '#description' => $settings['help_text'],
    '#autocomplete_path' => $settings['autocomplete_path'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $settings['button_text'],
  );

  return $form;
}

/**
 * SUBMIT: Quick Search form rendered via local__quick_search::view on Tripal Content Pages.
 *
 * @param $form
 *   Nested array of form elements that comprise the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 *
 * @return
 *   Completed $form above.
 */
function local__quick_search_field_view_form_submit($form, &$form_state) {

  $form_state['redirect'] = array(
    $form_state['values']['path'],
    array(
      'query' => array(
        $form_state['values']['key'] => $form_state['values']['value'],
      ),
    ),
  );

  dpm($form_state, 'form state');
}
