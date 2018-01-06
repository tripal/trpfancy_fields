<?php
/**
 * @class
 * Purpose: Provide a graphical summary of data stored in a materialized view.
 *   This is a generic, configurable fields to make it easier to add charts
 *   to Tripal Content pages.
 *
 * Data:
 *   Data is pulled from a configured materialized view and filtered based on a
 *   specific Tripal/Chado field.
 * Assumptions:
 *   - Currently there can only be one such chart per page. If you need more then
 *     one chart, a current work around is to extend this class using a different
 *     term.
 */
class local__single_series_chart extends ChadoChartField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Chado Single-Series Chart';

  // The default description for this field.
  public static $default_description = 'A type of graph in which all the data is part of a single series. Specifically, the dataset consists of labels and corresponding values.';

  // The default widget for this field.
  public static $default_widget = 'local__single_series_chart_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__single_series_chart_formatter';

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
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'single series chart',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'single series chart',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should ahve auto_attach set to FALSE so tha their values can be
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
    'filter_field' => 'taxrank__species',
    // Use this setting if the value you would like to filter on is the primary
    // key of the chado record being displayed on the page. For example, if you
    // want `organism_feature_count.organism_id = chado_record->organism_id`
    // then you set `use_record_id = TRUE` and `filter_column = organism_id`.
    'use_record_id' => FALSE,

    // The following relate to the materialized view:
    //-----------------------------------------------------
    // The table name of the materialized view to pull data from.
    'materialized_view' => 'organism_feature_count',
    // The column whose value should be the category labels on the chart.
    'label_column' => 'feature_type',
    // The column containing the numbers determining the category size.
    'count_column' => 'num_features',
    // The column to filter the materialized view based on. This is usually
    // a foreign key but doesn't have to be.
    'filter_column' => 'species',
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

}
