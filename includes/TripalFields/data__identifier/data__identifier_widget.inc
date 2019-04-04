<?php

/**
 * @class
 * Purpose: Provide a graphical summary of data stored in a materialized view.
 *   This is a generic, configurable fields to make it easier to add charts
 *   to Tripal Content pages.
 *
 * Allowing edit? No
 */
class data__identifier_widget extends ChadoFieldWidget {

  // The default lable for this field.
  public static $default_label = 'Autocomplete Uniquename';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('text');

  /**
   * Provides the form for editing of this field.
   *
   * This function corresponds to the hook_field_widget_form()
   * function of the Drupal Field API.
   *
   * This form is diplayed when the user creates a new entity or edits an
   * existing entity.  If the field is attached to the entity then the form
   * provided by this function will be displayed.
   *
   * At a minimum, the form must have a 'value' element.  For Tripal, the
   * 'value' element of a field always corresponds to the value that is
   * presented to the end-user either directly on the page (with formatting)
   * or via web services, or some other mechanism.  However, the 'value' is
   * sometimes not enough for a field.  For example, the Tripal Chado module
   * maps fields to table columns and sometimes those columns are foreign keys
   * therefore, the Tripal Chado modules does not just use the 'value' but adds
   * additional elements to help link records via FKs.  But even in this case
   * the 'value' element must always be present in the return form and in such
   * cases it's value should be set equal to that added in the 'load' function.
   *
   * @param $widget
   * @param $form
   *   The form structure where widgets are being attached to. This might be a
   *   full form structure, or a sub-element of a larger form.
   * @param $form_state
   *   An associative array containing the current state of the form.
   * @param $langcode
   *   The language associated with $items.
   * @param $items
   *   Array of default values for this field.
   * @param $delta
   *   The order of this item in the array of subelements (0, 1, 2, etc).
   * @param $element
   * A form element array containing basic properties for the widget:
   *  - #entity_type: The name of the entity the field is attached to.
   *  - #bundle: The name of the field bundle the field is contained in.
   *  - #field_name: The name of the field.
   *  - #language: The language the field is being edited in.
   *  - #field_parents: The 'parents' space for the field in the form. Most
   *    widgets can simply overlook this property. This identifies the location
   *    where the field values are placed within $form_state['values'], and is
   *    used to access processing information for the field through the
   *    field_form_get_state() and field_form_set_state() functions.
   *  - #columns: A list of field storage columns of the field.
   *  - #title: The sanitized element label for the field instance, ready for
   *    output.
   *  - #description: The sanitized element description for the field instance,
   *    ready for output.
   *  - #required: A Boolean indicating whether the element value is required;
   *    for required multiple value fields, only the first widget's values are
   *    required.
   *  - #delta: The order of this item in the array of subelements; see
   *    $delta above
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    // This widget applies only to type data identifier although it is attached to text
    // field which is a general field type. An warning if this is applied to other text field.

    if ($element['#field_name'] == 'data__identifier' && strtolower($element['#title']) == 'identifier') {
      // In identifier field.

      // Text field. In either cases this field must be disabled from user.
      $field_name = $this->field['field_name'];
      $field_table = $this->instance['settings']['chado_table'];
      $linker_field = 'chado-' . $field_table . '__uniquename';

      // When interface to set prefix:
      // Notice for admin where they can alter the prefix use to formulate uniquename.
      // @see hook_menu().
      $link = l('Update Prefix', 'admin/tripal/extension/trpfancy_fields/prefix');
      tripal_set_message('Administrators, you can set prefix used in uniquename field in ' . $link . '.', TRIPAL_INFO, array('return_html' => FALSE));

      // Dertermine if process is Create or Update. To test: check if chado_record_id
      // were set to an id number.
      if (isset($element['#entity']->chado_record_id) && $element['#entity']->chado_record_id > 0) {
        // Update here.

        $unique_name = array_key_exists($delta, $items) ? $items[$delta]['value'] : '';
        $desciption = '';

        // Feed to this field the uniquename for this stock.
        $widget['value'] = array(
          '#type' => 'value',
          '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
        );
      }
      else {
        // Create.

        // Pull system variable for configured prefix.
        $prefix = variable_get('trpfancy_field_identifier_prefix');

        if (isset($prefix) && !empty($prefix)) {
          $unique_name = $prefix . ' + ID #';
          $desciption = 'ID # will be set automatically on creation.';

          // Use this to flag process that user is creating a new record
          // and quick reference to the field.
          $form['trpfancy_fields_field_name'] = array(
            '#type' => 'value',
            '#value' => $field_name,
          );

          $widget['table'] = array(
            '#type' => 'value',
            '#value' => $field_table,
          );

          $id = uniqid();
          // What actually gets saved.
          $widget['value'] = array(
            '#type' => 'value',
            '#value' => $id,
          );
        }
      }

      // The value presented to the user via load.
      // If $items['delta']['value'] is set then we are updating and already have this
      // information. As such, simply save it again.

      // What user sees.
      $widget[$linker_field] = array(
        '#type'   => 'textfield',
        '#title'   => 'Uniquename',
        '#value'    => $unique_name,
        '#disabled'  => TRUE,
        '#description' => $desciption,
      );
    }
    else {
      // Text field but not identifier.
      drupal_set_message('This option is available only to Identifier (text field).', 'error');
      tripal_set_message('Administrators, please ensure that you are adding this autocomplete uniquename field only to Identifier field.', TRIPAL_INFO, array('return_html' => FALSE));
    }
  }

  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.
   * The 'value' key of this field must be set in the $form_state['values']
   * array anytime data is entered by the user.  It may be the case that there
   * are other fields for helping select a value. In the end those helper
   * fields must be used to set the 'value' field.
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
  }

  /**
   * Performs extra commands when the entity form is submitted.
   *
   * Drupal typically does not provide a submit hook for fields.  The
   * TripalField provides one to allow for behind-the-scenes actions to
   * occur.   This function should never be used for updates, deletes or
   * inserts for the Chado table associated with the field.  Rather, the
   * storage backend should be allowed to handle inserts, updates deletes.
   * However, it is permissible to perform inserts, updates or deletions within
   * Chado using this function.  Those operations can be performed if needed but
   * on other tables not directly associated with the field.
   *
   * An example is the chado.feature_synonym table.  The chado_linker__synonym
   * field allows the user to provide a brand new synonynm and it must add it
   * to the chado.synonym table prior to the record in the
   * chado.feature_synonym table.  This insert occurs in the widgetFormSubmit
   * function.
   *
   *  @param $entity_type
   *    The type of $entity.
   *  @param $entity
   *    The entity for the operation.
   *  @param $field
   *    The field structure for the operation.
   *  @param $instance
   *    The instance structure for $field on $entity's bundle.
   *  @param $langcode
   *    The language associated with $items.
   *  @param $items
   *    $entity->{$field['field_name']}[$langcode], or an empty array if unset.
   *  @param $form
   *    The submitted form array.
   *  @param $form_state.
   *    The form state array.
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    // Alter the value of the unique name to the unique id generated.
    // Once save, this temp uniquename will be used to query back the same record
    // to the final alteration of the uniquename to contain the primary key.
    if(isset($form_state['values']['trpfancy_fields_field_name'])) {
      // See callback function below.
      $field_name = $this->instance['field_name'];
      $field_table = $this->instance['settings']['chado_table'];
      $linker_field = 'chado-' . $field_table . '__uniquename';

      $tmp_uniquename = $form_state['values'][$field_name]['und'][0]['value'];

      $prefix = variable_get('trpfancy_field_identifier_prefix');
      $form_state['values'][$field_name][$langcode][$delta][$linker_field] = $prefix . $tmp_uniquename;
    }

    parent::submit($form, $form_state, $entity_type, $entity, $langcode, $delta);
  }
}

  /**
   * Function callback:
   * This function gets executed after submit() above and performs
   * the final alteration to the uniquename.
   */
  function trpfancy_fields_entity_insert($entity, $type) {
    // Apply this hook only to field of type Tripal Entity
    // and the data array contains the data_identifier field.
    if ($type == 'TripalEntity' && isset($entity->trpfancy_fields_field_name)) {
      $f = $entity->trpfancy_fields_field_name;
      $arr = $entity->$f;
      $data = $arr['und'][0];

      $tmp_uniquename = $data['value'];
      $prefix = variable_get('trpfancy_field_identifier_prefix');

      // Find the primary key of the table this uniquename is attached to.
      $pk = chado_query("SELECT column_name FROM information_schema.COLUMNS WHERE TABLE_NAME = :table",
        array(':table' => $data['table']))
        ->fetchField();

      $sql = sprintf("SELECT %s FROM {%s} WHERE uniquename = :uniquename LIMIT 1", $pk, $data['table']);
      $id = chado_query($sql, array(':uniquename' => $prefix . $tmp_uniquename))
        ->fetchField();

      if ($id) {
        // New id.
        $new_uniquename = $prefix . $id;

        chado_update_record($data['table'],
          array('uniquename' => $prefix . $tmp_uniquename),
          array('uniquename' => $new_uniquename)
        );
      }
    }
  }
