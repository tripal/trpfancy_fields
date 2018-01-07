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

    if (isset($items[0]) && !empty($items[0]['value'])) {

      // Load the Tripal D3 API
      tripald3_load_libraries();

      // Format the data.
      $rows = array();
      foreach ($items as $key => $value) {
        foreach ($value['value']['hydra:member'] as $part) {
          $rows[] = array(
            'label' => $part['rdfs:label'],
            'count' => number_format($part['local:count']),
          );
        }
      }

      // Finally, add the markup.
      $element[0] = array(
        '#type' => 'markup',
        '#theme' => 'table',
        '#header' => array('label' => 'Category', 'count' => 'Amount'),
        '#rows' => $rows,
        '#attributes' => array(
          'class' => array('tripald3-chart'),
        ),
        '#caption' => '<span class="tripald3-legend">'
          . '<span class="tripald3-title">Table: ' . $settings['title'] . '.</span> '
          . '<span class="tripald3-desc">' . $settings['legend'] . '</span>'
          . '</span>',
      );

    }
    else {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => 'No Data.'
      );
    }
  }

}
