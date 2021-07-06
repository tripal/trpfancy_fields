<?php
/**
 * @class
 * Purpose: Provide quick browse functionality for entity pages
 *
 * Data: No data.
 * Assumptions:
 */
class local__quick_browse extends TripalField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------
  // The default label for this field.
  public static $default_label = 'Quick Browse';
  // The default description for this field.
  public static $default_description = 'A simple quick browse which submits/redirects to an appropriately filtered search page.';
  // The default widget for this field.
  public static $default_widget = 'local__quick_browse';
  // The default formatter for this field.
  public static $default_formatter = 'local__quick_browse';
  // The module that manages this field.
  public static $module = 'trpfancy_fields';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'tripal_no_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'browseable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'quick_browse',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'quick_browse',
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
    'option_storage' => 'trpfancy_browse_options',
    // A list of browser types this field intends to provide.
    'browser_types' => '',
  ];

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

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];
    $instance_id = $this->instance['id'];

    // Retrieve the settings.
    $settings = $this->instance['settings'];

    // Based on configuration, the options will either be stored in
    // 1) trpfancy_browse_options:
    //    when options are global to all entities for the given field.
    if ($settings['option_storage'] == 'trpfancy_browse_options') {
      $query = 'SELECT * FROM {trpfancy_browse_options} WHERE instance_id=:id';
      $results = chado_query($query, [':id' => $instance_id]);

      foreach($results as $opt) {
        $entity->{$field_name}['und'][0]['value'][$opt->category][$opt->key] = $opt->value;
      }
    }

    // 2) trpfancy_browse_options_per_entity
    //    when options are specific to a given entity for the current field.

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

    // Simple radio buttons to select whether you want your options
    // per entity or constant for this field.
    $element['option_storage'] = [
      '#type' => 'radios',
      '#title' => 'Browser Option Source',
      '#options' => [
        'trpfancy_browse_options' => 'A. Constant for this Field',
        'trpfancy_browse_options_per_entity' => 'B. Dependant on the page shown',
      ],
      '#default_value' => $settings['option_storage'],
      '#required' => TRUE,
      '#description' => 'The browser field breaks down a set of options by
        "Browser Type". Depending on your choice here, the options can be a) constant
        for the field (e.g. same options for all organisms) or they can be
        b) specific to the page being shown (e.g. Specific to a given organism).',
    ];

    $element['browser_types'] = [
      '#type' => 'textarea',
      '#title' => 'Browser Types',
      '#required' => TRUE,
      '#description' => 'This field has a "Browse by [drop-down]" where the values in the
        drop-down will be the "Browser Types" you configure here. There will be a dedicated
        list of browse options for each "Browser Type" that will redirect the user to a
        partially filtered search. For Example, where "Browser Type" is "Type" on an organism
        page, the options might include "Gene","mRNA","Genetic Marker", "variant", etc.
        <strong>Each Browser Type should be indicated on a seperate line using the
        key=value format.</strong>',
      '#default_value' => $settings['browser_types'],
    ];

    $element['options'] = [
      '#type' => 'inline_template',
      '#template' => '<h1>Options</h1><p>Currently there is no UI for setting the option for a given browser type.
        Instead it is recommended you add them to the database manually either via "psql" or "drush sql-query".</p>
        <p>For example, the following query would insert 3 options for the browser type with
        the key="type1". The browse field would create 3 links: "Exome Capture", "Genotype-by-Sequencing",
        and "KASP". When the user clicks on a given link the "key" would be used in the URL
        defined in the settings on the "Manage Display" page.</p>
        <pre>
          INSERT INTO {trpfancy_browse_options} (instance_id, category, key, value) 
            VALUES
              ('.$this->instance['id'].', "type1", "1234", "Exome Capture"),
              ('.$this->instance['id'].', "type1", "2345", "Genotype-by-Sequencing"),
              ('.$this->instance['id'].', "type1", "3456", "KASP");
        </pre>
        <p>If you selected "A" for "Browser Option Source" above, this example query
        provides the options for all entities this field is attached to. If you selected
        "B" above, this example query provides a default option set for entities where
        a specific option set has not been set. To add the same options + one more
        for a specific entity you could use the following query (entity_id=5, instance_id='.$this->instance['id'].').</p>
        <pre>
          INSERT INTO {trpfancy_browse_options_per_entity} (entity_id, instance_id, category, key, value)
            VALUES
              (5, '.$this->instance['id'].', "type1", "1234", "Exome Capture"),
              (5, '.$this->instance['id'].', "type1", "2345", "Genotype-by-Sequencing"),
              (5, '.$this->instance['id'].', "type1", "3456", "KASP"),
              (5, '.$this->instance['id'].', "type1", "4567", "SCAR");
        </pre>
        <br />',
    ];


    // Show the user what options they have already set.
    $results = db_query('SELECT * FROM {trpfancy_browse_options} WHERE instance_id=:id ORDER BY category ASC, value ASC',
	  [':id' => $this->instance['id']]);
    $rows = [];
    foreach($results as $r) {
      $rows[] = [
        $r->category,
        $r->key,
        $r->value
      ];
    }
   
    if ($rows) {
      $element['options'] = [
        '#type' => 'table',
        '#header' => ['Browser Type', 'Key', 'Value'],
        '#rows' => $rows,
        '#empty' => t('0 Items in this table'),
      ];

      $element['options_markup'] = [
        '#type' => 'inline_template',
        '#template' => '<p><strong>Current Options</strong>: This table shows the default options you have for this field stored in trpfancy_browse_options.</p>'
      ];
    }
    else {
      $element['options_markup'] = [
        '#type' => 'inline_template',
        '#template' => '<p><strong>Current Options</strong>: You have no default options available for this field (instance_id: '.$this->instance['id'].'). Once you add them to the database following the instructions above, you will see them listed here.</p>'
      ];
    }

    return $element;
  }
}

/**
 * API: Process Browser Types
 *
 * @param $instance_settings
 *   The instance settings for this field.
 * @ return
 *   An array of browser types where key: machine name, value: human-readable name.
 */
function local__quick_browse_get_browser_types($instance_settings) {
  $types = [];

  $lines = explode("\n", $instance_settings['browser_types']);
  foreach ($lines as $line) {

    $line = trim($line);

    // Extract the key and value to add to the types list.
    $matches = [];
    if (preg_match('/^(\w+)=(\w+)$/', $line, $matches)) {
      $types[$matches[1]] = $matches[2];
    }
  }

  return $types;
}
