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
class co_010__parentage_of_pedigree_formatter extends TripalFieldFormatter {
  // The default label for this field.
  public static $default_label = 'Parental Pedigree';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('co_010__parentage_of_pedigree');

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
    $display_vars = $entity->{$field_name}['und'][0]['vars'];

    // Prepare settings:
    drupal_add_js(array('trpfancyFields' => array(
      // Stock name.
      'name'   => $display_vars['data']['name'],
      // JSON url.
      'url'    => $display_vars['source'],
      // Append SVG element and configurations.
      'chart'  => $display_vars['chart']),

    ), 'setting');


    // Initialize chart:
    // Load Tripal D3 API.
    tripald3_load_libraries();

    drupal_add_js($display_vars['source']['path_T3field'] . '/theme/script.js',
      array(
        // An external file scrip.js in theme/
        'type' => 'file',
        // At foot of DOM to ensure document is ready for width estimate.
        'scope' => 'footer'
      )
    );


    // Add element:
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => '
        <div id="' . $display_vars['chart']['wrapper']  . '">
          <div id="tripald3-pedigree-wait">Loading pedigree tree diagram. Please wait...</div>
          <div id="' . $display_vars['chart']['element'] . '"></div>
        </div>',
    );


    return $element;
  }
}
