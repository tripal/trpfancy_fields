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
class PieChartField extends TripalField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Tripal Chart';

  // The default description for this field.
  public static $default_description = 'Provides a graphical summary of data.';

  // The default widget for this field.
  public static $default_widget = 'NoWidget';

  // The default formatter for this field.
  public static $default_formatter = 'PieChartFormatter';

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

    // The pie chart shown is able to be specific to the Drupal page by using a
    // Drupal/Tripal/Chado field to filter the materialized view. For example,
    // if you are on the "Tripalus databasica" organism page, then you may want
    // a pie chart showing the ratio/count of "Tripalus databasica" sequence
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
    // The column containing the numbers determining the pie piece size.
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

    // @debug ddl('executed load');

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
      $filter_value = $entity->{$filter_field}['und'][0]['value'];
    }

    // Then grab the information we need from the materialized view
    // and add it as the value.
    $query = "SELECT $label_column, $count_column
              FROM {".$table_name."}
              WHERE $filter_column=:filter_value
              ORDER BY $count_column DESC";
    $results = chado_query($query, array(':filter_value' => $filter_value));
    $entity->local__chart['und'][0]['value'] = array();
    $entity->local__chart['und'][0]['value']['hydra:member'] = array();
    foreach ($results as $r) {
      $entity->local__chart['und'][0]['value']['hydra:member'][] = array(
        'rdfs:label' => $r->{$label_column},
        'local:count' => $r->{$count_column},
      );
    }
  }
}
