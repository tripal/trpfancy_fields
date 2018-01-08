<?php
/**
 * @class
 * Purpose: Provide a graphical summary of data stored in a materialized view.
 *   This is a generic, configurable field to make it easier to add charts
 *   to Tripal Content pages.
 *
 * *** MUST BE EXTENDED ***
 * NULL Values in $default_instance_settings must be set in the child class.
 *
 * Data:
 *   Data is pulled from a configured materialized view and filtered based on a
 *   specific Tripal/Chado field.
 * Assumptions:
 *   - None currently identified.
 */
class ChadoChartField extends TripalField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Chado Chart';

  // The default description for this field.
  public static $default_description = 'Provides a graphical summary of data.';

  // The default widget for this field.
  public static $default_widget = NULL;

  // The default formatter for this field.
  public static $default_formatter = NULL;

  // The module that manages this field.
  public static $module = 'trpfancy_fields';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'tripal_no_storage',
     // It is expected that all fields set a 'value' in the load() function.
     // In many cases, the value may be an associative array of key/value pairs.
     // In order for Tripal to provide context for all data, the keys should
     // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
     // function that are supported by the query() function should be
     // listed here.
     'searchable_keys' => array(),
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => NULL,
    // The name of the term.
    'term_name' => NULL,
    // The unique ID (i.e. accession) of the term.
    'term_accession' => NULL,
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asyncronously.
    'auto_attach' => FALSE,

    // The chart shown is able to be specific to the Drupal page by using a
    // Drupal/Tripal/Chado field to filter the materialized view. For example,
    // if you are on the "Tripalus databasica" organism page, then you may want
    // a chart showing the ratio/count of "Tripalus databasica" sequence
    // features. This is done using a combination of the filter_field (Drupal)
    // and filter_column (Materialized View).
    //-----------------------------------------------------
    // Use this setting to specify the Drupal/Tripal/Chado field containing the
    // value to filter the materialized view on. For example, if you want
    // `organism_feature_count.species = taxrank__species` then you set the
    // `filter_field = taxrank__species` and the `filter_column = species`.
    'filter_field' => NULL,
    // Use this setting if the value you would like to filter on is the primary
    // key of the chado record being displayed on the page. For example, if you
    // want `organism_feature_count.organism_id = chado_record->organism_id`
    // then you set `use_record_id = TRUE` and `filter_column = organism_id`.
    'use_record_id' => FALSE,

    // The following relate to the materialized view:
    //-----------------------------------------------------
    // The table name of the materialized view to pull data from.
    'materialized_view' => NULL,
    // The column whose value should be the category labels on the chart.
    'label_column' => NULL,
    // The column containing the numbers determining the category size.
    'count_column' => NULL,
    // The column to filter the materialized view based on. This is usually
    // a foreign key but doesn't have to be.
    'filter_column' => NULL,
  );

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web serivces or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

 /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the enity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($entity) {

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];

    // Retrieve the default settings for use when loading data.
    $table_name = $this->instance['settings']['materialized_view'];
    $label_column = $this->instance['settings']['label_column'];
    $count_column = $this->instance['settings']['count_column'];
    $filter_column = $this->instance['settings']['filter_column'];
    $filter_field = $this->instance['settings']['filter_field'];
    $use_record_id = $this->instance['settings']['use_record_id'];

    // Grab the primary key of the chado record.
    if ($use_record_id) {
      $filter_value = $entity->chado_record_id;
    }
    // Or grab the value of a configured Dripal/Tripal/Chado field.
    else {
      if (!empty($filter_field) && isset($entity->{$filter_field})) {
        $filter_value = $entity->{$filter_field}['und'][0]['value'];
      }
      else {
        tripal_report_error(
          'trpfancy_fields',
          TRIPAL_ERROR,
          'Attempted to display %field but there was no value to filter on. Specifically, either field %filter_field had no value or a filter field was not specified.',
          array('%field' => $field_name, '%filter_field' => $filter_field)
        );
        return FALSE;
      }
    }

    // Then grab the information we need from the materialized view
    // and add it as the value.
    $query = "SELECT $label_column, $count_column
              FROM {".$table_name."}
              WHERE $filter_column=:filter_value
              ORDER BY $count_column DESC";
    $results = chado_query($query, array(':filter_value' => $filter_value));
    $entity->{$field_name}['und'][0]['value'] = array();
    $entity->{$field_name}['und'][0]['value']['hydra:member'] = array();
    foreach ($results as $r) {
      $entity->{$field_name}['und'][0]['value']['hydra:member'][] = array(
        'rdfs:label' => $r->{$label_column},
        'local:count' => $r->{$count_column},
      );
    }
  }

  /**
   * Provides a form for the 'Field Settings' of an instance of this field.
   *
   * This function corresponds to the hook_field_instance_settings_form()
   * function of the Drupal Field API.
   *
   * Validation of the instance settings form is not supported by Drupal, but
   * the TripalField class does provide a mechanism for supporting validation.
   * To allow for validation of your setting form you must call the parent
   * in your child class:
   *
   * @code
   *   $element = parent::instanceSettingsForm();
   * @endcode
   *
   * Please note, the form generated with this function does not easily
   * support AJAX calls in the same way that other Drupal forms do.  If you
   * need to use AJAX you must manually alter the $form in your ajax call.
   * The typical way to handle updating the form via an AJAX call is to make
   * the changes in the form function itself but that doesn't work here.
   */
  public function instanceSettingsForm() {

    // Retrieve the current settings.
    // If this field was just created these will contain the default values.
    $settings = $this->instance['settings'];

    // Allow the parent Tripal Field to set up the form element for us.
    $element = parent::instanceSettingsForm();

    // The chart shown is able to be specific to the Drupal page by using a
    // Drupal/Tripal/Chado field to filter the materialized view. For example,
    // if you are on the "Tripalus databasica" organism page, then you may want
    // a chart showing the ratio/count of "Tripalus databasica" sequence
    // features. This is done using a combination of the filter_field (Drupal)
    // and filter_column (Materialized View).
    //-----------------------------------------------------
    $element['data'] = array(
      '#type' => 'fieldset',
      '#title' => 'Chart Data Source',
      '#description' => 'The data for this chart will be pulled from the materialized view specified below. Additonally you must configure which columns in the materialized view should be the source of the data and which should be used to filter the results.',
    );

    // The following relate to the materialized view:
    //-----------------------------------------------------
    // The table name of the materialized view to pull data from.
    $element['data']['materialized_view'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Materialized View',
      '#description' => 'The materialized view you would like to pull the data for the chart from.',
      '#default_value' => $settings['materialized_view'],
      '#weight' => -10,
    );

    $element['data']['filter_msg'] = array(
      '#type' => 'item',
      '#markup' => 'The materialized view indicated likely has data for multiple Tripal Content Pages. Thus, to ensure the chart is specific to the current page, we must filter the materialized view. This is done by indicated a column in the materialized view (the filter column) that should equal the value of a Drupal/Tripal/Chado field (the filter field) for the current page.',
      '#weight' => 100,
    );

    // The column to filter the materialized view based on. This is usually
    // a foreign key but doesn't have to be.
    $element['data']['filter_column'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Filter Column',
      '#description' => 'The column from the materialized view whose value should equal the value of the filter field or the chado record ID indicated above.',
      '#default_value' => $settings['filter_column'],
      '#weight' => 101
    );

    // Drupal/Tripal/Chado field to filter the materialized view on.
    $element['data']['filter_field'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Filter Field',
      '#description' => 'Use this setting to specify the Drupal/Tripal/Chado field containing the value to filter the materialized view on. For example, if you want records from the materialized view with the value of the taxrank__species Chado field then select that field here. <strong>This will be ignored if you select the "Use Chado Record ID" checkbox below.</strong>',
      '#default_value' => $settings['filter_field'],
      '#weight' => 102,
    );

    // Use the chado primary key instead of the filter field above.
    $element['data']['use_record_id'] = array(
      '#type' => 'checkbox',
      '#title' => 'Use Chado Record ID',
      '#description' => 'Use this setting if the value you would like to filter on is the primary key of the chado record being displayed on the page. For example, if you are adding this chart to organism pages and you would like the organism_id for the current page to be used to filter the materialized view, then select this checkbox.',
      '#default_value' => $settings['use_record_id'],
      '#weight' => 103,
    );

    return $element;
  }
}
