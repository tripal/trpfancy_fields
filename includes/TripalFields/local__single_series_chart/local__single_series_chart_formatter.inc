<?php
/**
 * @class
 * Purpose: Provide a table summary of data stored in a materialized view.
 *   This is a generic, configurable fields to make it easier to add summaries
 *   to Tripal Content pages.
 *
 * Display:
 *   Draws a table summarizing the data.
 * Configuration:
 *   None yet available.
 */
class local__single_series_chart_formatter extends ChadoChartFormatter {

  // The default lable for this field.
  public static $default_label = 'Single-Series Table';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__single_series_chart'];

  // The list of default settings for this formatter.
  // Due to the different charts available and the dependance upon the data,
  // it is too difficult to make a good default for the title and legend.
  // As such I am making a horrible one in the hopes of making admin change it ;-p.
  public static $default_settings = [
    'title' => 'Fancy Chart summarizing Stuff',
    'legend' => 'This should describe the chart including the data that goes into it and how to interpret that data. Furthermore, it is always a good idea to highlight how users can interact with this chart.',
    // Supports either 'top' or 'bottom'. This refers to the legend only,
    // title is always at the top.
    'legend_pos' => 'top',
    // Supports either 'data' or 'info'.
    'table_type' => 'data',
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
    if (!isset($settings['legend_pos'])) { $settings['legend_pos'] = self::$default_settings['legend_pos']; }
    if (!isset($settings['table_type'])) { $settings['table_type'] = self::$default_settings['table_type']; }

    $element['legend_pos'] = [
      '#type' => 'radios',
      '#title' => 'Legend Postion',
      '#description' => 'The position of the "Figure Legend" text filled out above in respect to the table itself.',
      '#options' => ['top' => 'top', 'bottom' => 'bottom'],
      '#default_value' => $settings['legend_pos'],
    ];

    $element['table_type'] = [
      '#type' => 'radios',
      '#title' => 'Table Type',
      '#description' => 'The type of table to generate where <strong>data</strong> refers to the typical data table with header on top and <strong>info</strong> refers to the typical Tripal info tables where the header is down the side and consists of your category labels.',
      '#options' => ['data' => 'data', 'info' => 'info'],
      '#default_value' => $settings['table_type'],
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

    if (isset($items[0]) && !empty($items[0]['value'])) {

      // Load the Tripal D3 Libraries.      
      $element['#attached']['library'][] = 'tripald3/D3';
      $element['#attached']['library'][] = 'tripald3/D3';
      
      // CORE
      $element['#attached']['library'][] = 'tripald3/D3';
      $element['#attached']['library'][] = 'tripald3/tripalD3';
      
      // CHARTS
      // Pie
      $element['#attached']['library'][] = 'tripald3/lib-pie';
      $element['#attached']['library'][] = 'tripald3/lib-bar';
      'tripald3/lib-pedigree',
      
      // CREATE
      $element['#attached']['library'][] = 'tripald3/create-multidonut';
      $element['#attached']['library'][] = 'tripald3/create-pie';
      $element['#attached']['library'][] = 'tripald3/create-multiseriesdonut';
      $element['#attached']['library'][] = 'tripald3/create-bar';
      $element['#attached']['library'][] = 'tripald3/create-pedigree';

      // STYLE
      $element['#attached']['library'][] = 'tripald3/style-tripald3';

      // SETTINGS 
      // Color scheme configuration.
      $default_scheme = \Drupal::service('tripald3.TripalD3ColorScheme')
        ->registerColorScheme();
      $to_Drupalsettings['tripalD3']['vars']['colorScheme'] = $default_scheme; 

      // Auto resize configuration.        
      $default_resize = $this->config('tripald3.settings')
        ->get('tripald3_autoResize');
      $to_Drupalsettings['tripalD3']['vars']['autoResize']  = $default_resize;
      
      $element['#attached']['drupalSettings'] = $to_Drupalsettings;


      // Defaults based on table type.
      //  -- Info.
      if ($settings['table_type'] == 'info') {
        // If the table type is info we don't need a traditional header, because
        // our data includes the header (first column).
        $header = NULL;
        $column_is_header = TRUE;
      }

      // -- Data (default).
      else {
        $header = ['label' => 'Category', 'count' => 'Amount'];
        $column_is_header = FALSE;
      }

      // Legend Position.
      if ($settings['legend_pos'] == 'bottom') {
        $caption = '<span class="tripald3-legend"><span class="tripald3-title">'.$settings['title'].'</span></span>';
        $footer = '<span class="tripald3-legend"><span class="tripald3-desc">'.$settings['legend'].'</span></span>';
      }
      else {
        $footer = FALSE;
        $caption = '<span class="tripald3-legend">'
          . '<span class="tripald3-title">Table: ' . $settings['title'] . '.</span> '
          . '<span class="tripald3-desc">' . $settings['legend'] . '</span>'
          . '</span>';
      }

      // Format the data.
      $rows = [];
      foreach ($items as $key => $value) {
        foreach ($value['value']['hydra:member'] as $part) {

          // Compile the row.
          $row = [
            'label' => ['data' => $part['rdfs:label']],
            'count' => number_format($part['local:count']),
          ];

          // Indicate if the first column should be a header.
          if ($column_is_header) {
            $row['label']['header'] = TRUE;
          }

          // Finally, add the current row to the table.
          $rows[] = $row;
        }
      }

      // Finally, add the markup.
      $element[0] = [
        '#type' => 'table',
        '#title' => $caption,
        '#header' => $header,
        '#rows' => $rows,
        '#empty' => t('0 Items in this table'),
        '#attributes' => [
          'class' => ['tripald3-chart', $settings['table_type']],
        ]
      ];

      if ($footer) {
        $element[0]['#suffix'] = '<div class="trpfancy-table-footer">'.$footer.'</div>';
      }
    }
  }
}
