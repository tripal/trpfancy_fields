<?php
use Drupal\Core\Url;

/**
 * @class
 * Purpose: Provide a quick browse on entity pages which submits/redirects to a full browse.
 *
 * Display: A simple textfield browse form.
 * Configuration:
 *   - path to the full browse.
 *   - the URL token (query parameter) the value applies to.
 *   - help text.
 *   - textfield placeholder.
 *   - browse button text.
 *   - autocomplete path.
 */
class local__quick_browse_formatter extends TripalFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Simple Quick Browse';
  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__quick_browse'];
  // The list of default settings for this formatter.
  public static $default_settings = [
    'button_text' => 'Browse',
    'help_text' => 'Choose what you would like to browse from the drop-down. Clicking one of the resulting links will take you to a listing for further browsing.',
    'categories' => [],
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

    // Grab the parent settings form in all it's glory.
    $element = parent::settingsForm($view_mode, $form, $form_state);

    // Retrieve the current settings not handled by our parent.
    $settings = $this->instance['display'][$view_mode]['settings'];

    // Process the Browser Types set in the "Edit Field" form.
    $types = local__quick_browse_get_browser_types($this->instance['settings']);

    // Ensure each browser type has a config form in the categories.
    foreach ($types as $k => $v) {
      if (!isset($settings['categories'][$k])) {
        $settings['categories'][$k] = [
          'type' => $v,
          'path' => NULL,
          'key' => NULL,
          'filter_qstring' => NULL,
        ];
      }
      else {
        $settings['categories'][$k]['type'] = $v;
      }
    }

    // This has caught me a couple of times so I'm going to remind both myself and others ;-p.
    $element['msg'] = [
      '#type' => 'inline_template',
      '#template' => '<div class="messages warning">Settings are only saved after you click "Update" within this box <br />AND "Save" at the bottom of the page!</div>',
    ];

    $element['categories'] = [
      '#type' => 'details',
      '#title' => 'Browser Types',
      '#description' => 'This field has a "Browse by [drop-down]" where the values in the
        drop-down will be the "Browser Types" you configure here. There will be a dedicated
        list of browse options for each "Browser Type" that will redirect the user to a
        partially filtered search. For Example, where "Browser Type" is "Type" on an
        organism page, the options might include "Gene","mRNA","Genetic Marker", "variant", etc.',
    ];

    foreach ($settings['categories'] as $category => $category_settings) {
      $element['categories'][$category] = [
        '#type' => 'fieldset',
        '#title' => $category_settings['type'],
      ];

      $element['categories'][$category]['type'] = [
        '#type' => 'hidden',
        '#value' => $category_settings['type'],
      ];

      $element['categories'][$category]['path'] = [
        '#type' => 'textfield',
        '#title' => 'Browse Path',
        '#required' => TRUE,
        '#description' => 'This is the relative path of the browse page you would like to redirect the user to.',
        '#default_value' => $category_settings['path'],
      ];

      $element['categories'][$category]['key'] = [
        '#type' => 'textfield',
        '#title' => 'Query Parameter Key',
        '#required' => TRUE,
        '#description' => 'This key is used in the query parameters to tell your full browse which filter to apply the value to.',
        '#default_value' => $category_settings['key'],
      ];

      $element['categories'][$category]['filter_qstring'] = [
        '#type' => 'textarea',
        '#title' => 'Additional Filter Criteria',
        '#description' => 'Use this field to add additional filter criteria to the URL query string upon submimt. Each additional criteria should be indicated on a seperate line using the key=value format. Tokens of the format `[field machine name]` can be used to make these filter dynamic.',
        '#default_value' => $category_settings['filter_qstring'],
      ];
    }

    $element['button_text'] = [
      '#type' => 'textfield',
      '#title' => 'Button Text',
      '#required' => TRUE,
      '#description' => 'The text on the browse button.',
      '#default_value' => $settings['button_text'],
    ];

    $element['help_text'] = [
      '#type' => 'textarea',
      '#title' => 'Help Text',
      '#description' => 'Guidance to your users for how to use the quick browse form.',
      '#default_value' => $settings['help_text'],
    ];

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

    // Warn the admin if the field has not been configured.
    if (empty($settings['categories'])) {
      tripal_report_error(
        'trpfancy_fields',
        TRIPAL_ERROR,
        'No Browser types available. Please configure the field, :field_name, <a href="@url">here</a>.',
        [':field_name' => $this->instance['field_name'], '@url' => url('/admin/structure/bio_data/manage/'.$this->instance['bundle'].'/display')]
      );
      return $element;
    }

    // Warn the admin if there are no options for this field.
    if (empty($items)) {
      // How concerned we are depends on the type of storage chosen.
      // If there is only one set of options for all entities of this type
      // Then we are quire concerned...
      if ($this->instance['settings']['option_storage'] == 'trpfancy_browse_options') {
        tripal_report_error(
          'trpfancy_fields',
          TRIPAL_ERROR,
          'No options available for :field_label (:field_name). Please add options by editing the field <a href="@url">here</a>.',
          [':field_label' => $this->instance['label'], ':field_name' => $this->instance['field_name'], '@url' => url('/admin/structure/bio_data/manage/'.$this->instance['bundle'].'/fields/'.$this->instance['field_name'])]
        );
      }
      // However, if there are options per entity then it may just be that this particular
      // entity shouldn't have options.
      else {
        tripal_report_error(
          'trpfancy_fields',
          TRIPAL_NOTICE,
          'No options available for :entity_title. Please add options by editing the entity, <a href="@url">here</a>, and filling out :field_label in the form.',
          [':entity_title' => $entity->title, ':field_label' => $this->instance['label'], '@url' => url('/bio_data/'.$entity->id.'/edit')]
        );
      }
      return $element;
    }

    // Save the options to be passed through to the form later.
    $options = $items[0]['value'];

    $browser_types = [];
    foreach ($settings['categories'] as $type => $info) {

      // If there are no options for the current browser type then don't add it.
      if (!isset($options[ $type ])) {
        continue;
      }

      $browser_types[$type] = $info['type'];

      // Process the additional query params.
      // We do this here so we can fill in the values of the fields
      // and so we don't increase time during the redirect.
      // Start our main paramter which will later be entered by the user.
      $settings['categories'][$type]['qparams'] = [$info['key'] => '[value]'];

      // Then, if there are any additional query params, we will process them line-by-line.
      // @assumption: each additional query param is on it's own line.
      if (!empty($info['filter_qstring'])) {

        $lines = explode("\n", $info['filter_qstring']);
        foreach ($lines as $line) {

          $line = trim($line);

          // Check if there's a field token & if so, replace it.
          $matches = [];
          if (preg_match('/\[(\w+)\]/', $line, $matches)) {
            $key = '[' . $matches[1] . ']';
            $values = field_get_items($entity_type, $entity, $matches[1]);
            if ($values === FALSE) {
              tripal_report_error('trpfancy_fields', TRIPAL_WARNING,
                'Quick Browse Field %name: %token not found, replaced with empty string.',
                ['$name' => $this->instance['field_name'], '%token' => $key]);
            }

            foreach ($values as $v) {
              $value = $v['safe_value'];
              $line = str_replace($key, $value, $line);
            }
          }

          // Extract the key and value to add to the params list.
          $matches = [];
          if (preg_match('/^(\w+\[*\]*)=(\w+)$/', $line, $matches)) {
            $settings['categories'][$type]['qparams'][$matches[1]] = $matches[2];
          }
        }
      }
    }

    // Retrieve my form (implemented below this class).
    $form = drupal_get_form(
      'local__quick_browse_field_view_form',
      [
        'settings' => $settings,
        'options' => $options,
        'browser_types' => $browser_types,
      ]
    );

    // Render it and pass it as the markup for this field.
    $element[0] = [
      '#type' => 'markup',
      '#markup' => drupal_render($form),
    ];

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

    $types = [];
    foreach ($settings['categories'] as $category => $values) {
      $types[] = $values['type'];
    }

    return '<strong>Browser Types:</strong> ' . implode(', ',$types);

  }

}

/**
 * Quick Browse form rendered via local__quick_browse::view on Tripal Content Pages.
 *
 * @param $form
 *   Nested array of form elements that comprise the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 *
 * @return
 *   Completed $form above.
 */
function local__quick_browse_field_view_form($form, &$form_state) {

  // Retrieve the formatter settings from the build info.
  $settings = $form_state['build_info']['args'][0]['settings'];
  $options = $form_state['build_info']['args'][0]['options'];
  $browser_types = $form_state['build_info']['args'][0]['browser_types'];

  $form['#attached']['css'][] = [
    'type' => 'inline',
    'data' => '
      #local-quick-browse-field-view-form {
        margin-top: 10px;
      }
      #local-quick-browse-field-view-form .description {
        font-size: 0.9em;
        font-style: italic;
        display: block;
      }
      #local-quick-browse-field-view-form .item-list markup {
        display: flex;
        flex-wrap: wrap;
        list-style-type: none;
        margin: 10px;
      }
      #local-quick-browse-field-view-form .item-list markup li {
        flex: 1 0 25%;
        box-sizing: border-box;
        padding: 10px;
      }
      #local-quick-browse-field-view-form .form-item-type label {
        display: inline;
      }
      #local-quick-browse-field-view-form .form-item-type select {
        width: 300px;
      }
    ',
  ];

  // Choose a default browser type ;-p.
  reset($browser_types);
  $default_type = key($browser_types);

  $form['type'] = [
    '#type' => 'select',
    '#title' => 'Browse',
    '#options' => $browser_types,
    '#empty_option' => ' - Select -',
    '#description' => $settings['help_text'],
    '#default_value' => $default_type,
  ];

  $form['path'] = [
    '#type' => 'hidden',
    '#value' => $settings['categories'][$current_type]['path'],
  ];

  $form['key'] = [
    '#type' => 'hidden',
    '#value' => $settings['categories'][$current_type]['key'],
  ];

  $form['qparams'] = [
    '#type' => 'hidden',
    '#value' => $settings['categories'][$current_type]['qparams'],
  ];

  foreach (array_keys($browser_types) as $current_type) {
    $list = [];
    foreach($options[$current_type] as $k => $v) {
      $params = $settings['categories'][$current_type]['qparams'];
      $params[ $settings['categories'][$current_type]['key'] ] = $k;

      $link = \Drupal::l(t($v), Url::fromUri($settings['categories'][$current_type]['path'], ['query' => $params]));
      $list[] = $link;
    }

    $form['options'][$current_type] = [
      '#theme' => 'item_list',
      '#list_type' => 'ul',
      '#items' => $list,
      '#prefix' => '<div id="browse-options-'.str_replace(' ' ,'-',$current_type).'" class="browser-options" style="display:none;">',
      '#suffix' => '</div>',
    ];
  }

  // Finally add javascript that will show/hide based on select box.
  // Note: We used js here rather then ajax because drupal ajax had trouble finding the form.
  $form['#attached']['html_head'][] = [
    [
      '#tag' => 'script',
      '#attributes' => ['type' => 'text/javascript'],
      '#value' => '
        Drupal.behaviors.trpfancyFieldsQuickBrowse = {
          attach: function (context, settings) {

            // If the selectbox is set on page load, we should show the appropriate options.
            var selected = document.getElementById("edit-type").value;
            jQuery("#browse-options-"+selected).show();

            // Ensure that when the selectbox is changed, the options change.
            document.getElementById("edit-type").onchange = function (e) {
              // Hide all browser options
              jQuery(".browser-options").hide();
              // Show the browser options by removing display:none.
              jQuery("#browse-options-"+this.value).show();
            };
        }};
      '
    ],
  ];

  return $form;
}

/**
 * SUBMIT: Quick Browse form rendered via local__quick_browse::view on Tripal Content Pages.
 *
 * @param $form
 *   Nested array of form elements that comprise the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 *
 * @return
 *   Completed $form above.
 */
function local__quick_browse_field_view_form_submit($form, &$form_state) { }