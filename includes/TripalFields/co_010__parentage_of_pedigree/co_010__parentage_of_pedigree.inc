<?php
/**
 * @class
 * Purpose: Provide quick browse functionality for entity pages
 *
 * Data: No data.
 * Assumptions:
 */
class co_010__parentage_of_pedigree extends TripalField {
  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------
  // The default label for this field.
  public static $default_label = 'Parental Pedigree';
  // The default description for this field.
  public static $default_description = 'A tree diagram of parental pedigree';
  // The default widget for this field.
  public static $default_widget = 'co_010__parentage_of_pedigree_widget';
  // The default formatter for this field.
  public static $default_formatter = 'co_010__parentage_of_pedigree_formatter';
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
    'browseable_keys' => array(),
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'CO_010',
    // The name of the term.
    'term_name' => 'parentage of pedigree',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000220',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table where the options for this specific field are stored.
    // This can be one of trpfancy_browse_options or trpfancy_browse_options_per_entity
    // based on admin configuration. Default: trpfancy_browse_options.
    'option_storage' => '',
    // A list of browser types this field intends to provide.
    'browser_types' => '',
  );

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;
  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick browse field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = TRUE;

 /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
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
    // The record.
    $chado_record_id = $entity->chado_record_id;

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];

    // Set this property for formatteer to render.
    $entity->{$field_name}['und'][0]['value']['GCP germplasm ontology:parentage of pedigree'] = '';


    // Chart configuration:
    $chart = array(
      // Whether to put figure key legend or not.
      // TRUE of FALSE;
      'draw_key_legend' => TRUE,
      // Where to put figure key legend.
      // left, right or top
      'key_legend_position' => 'top',
      // On page load, default number of level to collapse.
      // From end leaf down to root.
      'collapse_level' => 3,

      // Element #ids. Any future requirements to add element
      // can be inserted into this parent container.
      'wrapper' => 'tripald3-pedigree-wrapper',
      // Append svg to.
      'element' => 'tripald3-pedigree'
    );


    // Data:
    $data = array(
      'name' => $entity->title,
    );


    // Source:
    $path_base     = $GLOBALS['base_url'];
    $path_to_module = drupal_get_path('module', 'trpfancy_fields');
    $path_to_tfield  = $path_base . '/' . $path_to_module . '/includes/TripalFields';
    $path_to_pedigree = $path_to_tfield . '/' . get_class($this);
    $path_to_JSON      = $path_base . '/ajax/tripal/d3-json/relationships/stock/' . $chado_record_id;

    // Germplasm type.
    $stock_type = $entity->chado_record->type_id->name;
    $path_to_germpage   = $path_base . '/' . strtolower($stock_type) . '/';

    $source = array(
      'path_JSON' => $path_to_JSON,
      'path_T3field' => $path_to_pedigree,
      'path_germpage'  => $path_to_germpage,
    );



    $entity->{$field_name}['und'][0]['vars'] = array(
      'chart'  => $chart,
      'data'   => $data,
      'source' => $source,
    );
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

    return $element;
  }
}

