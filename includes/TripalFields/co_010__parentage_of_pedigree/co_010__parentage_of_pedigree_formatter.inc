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
  public static $field_types = ['co_010__parentage_of_pedigree'];

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

    // Add user login status. This will be used to restrict data.
    // This is very important to be in in this class and not in the load function
    // which, at first I tried it in that method.
    // The problem is it caches the value so that when youre logged in
    // user_is_looged_in alwasy returns 1.
    $pass = (user_is_logged_in()) ? 1 : 0;

    // Initialize chart:
    // Load Tripal D3 API.
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
    
    $element['#attached']['drupalSettings'][] = $to_Drupalsettings;

    // Prepare settings:
    $field_settings = [
      'trpfancyFields' => [
        // Stock name.
        'name'   => $display_vars['data']['name'],
        // JSON url.
        'url'    => $display_vars['source'],
        // Append SVG element and configurations.
        'chart'  => $display_vars['chart'],
        // Authenticate user.
        'pass'   => $pass
      ]
    ];

    $element['#attached']['drupalSettings'][] = $field_settings;
    // FIELD SCRIPT.
    $element['#attached']['library'][] = 'trpfancy_fields/tripal-fancyfields-co-010';


    // Add element:
    $element[0] = [
      '#type' => 'inline_template',
      '#template' => '
        <div id="' . $display_vars['chart']['wrapper']  . '">
          <div id="tripald3-pedigree-wait">Loading pedigree tree diagram. Please wait...</div>
          <div id="' . $display_vars['chart']['element'] . '"></div>
        </div>',
    ];


    return $element;
  }
}
