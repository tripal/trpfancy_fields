<?php
/**
 * @class
 * Purpose: Provide a graphical summary of data stored in a materialized view.
 *   This is a generic, configurable fields to make it easier to add charts
 *   to Tripal Content pages.
 *
 * Display:
 *   Draws a bar chart using D3.js
 * Configuration:
 *   None yet available.
 */
class local__single_series_bar_chart_formatter extends ChadoChartFormatter {

  // The default lable for this field.
  public static $default_label = 'Single-Series Bar Chart';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__single_series_chart'];

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
      $data = [];
      foreach ($items as $key => $value) {
        foreach ($value['value']['hydra:member'] as $part) {
          $data[] = [
            'label' => $part['rdfs:label'],
            'count' => $part['local:count'],
          ];
        }
      }

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
      $to_Drupalsettings['trpfancyFields'][ $field_name ] = $data;

      $element['#attached']['drupalSettings'] = $to_Drupalsettings;

      $element['#attached']['html_head'][] = [
        [      
          '#tag' => 'script',
          '#attributes' => ['type' => 'text/javascript'],
          '#value' => '
            Drupal.behaviors.'.$field_name.' = {
              attach: function (context, settings) {

                // Pull the data out of the javascript settings.
                var data = drupalSettings.trpfancyFields.'.$field_name.';

                // Draw your chart.
                tripalD3.drawFigure(
                  data,
                  {
                    "chartType" : "simplebar",
                    "elementId": "trpfancy-'.$field_name.'",
                    "height": 250,
                    "width": 500,
                    "keyPosition": "right",
                    "title": "' . $settings['title'] . '",
                    "legend": "' . $settings['legend'] . '",
                  }
                );
              }
            };'
        ],
      ];

      // Finally, add the markup.
      $element[0] = [
        '#type' => 'inline_template',
        '#template' => '
          <div id="trpfancy-'.$field_name.'">
            <!-- JS should draw the bar chart here -->
          </div>',
      ];
    }
  }
}
