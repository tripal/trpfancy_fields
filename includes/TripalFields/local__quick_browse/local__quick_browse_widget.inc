<?php

// Include the field since it has some API functionas at the bottom ;-).
include_once('local__quick_browse.inc');

/**
 * @class
 * Purpose: Provide a quick browse on entity pages which submits/redirects to a full browse.
 *
 * Allowing edit? Edits the options.
 * Choice for option_storage depends where the options are stored.
 */
class local__quick_browse_widget extends TripalFieldWidget {

  // The default lable for this field.
  public static $default_label = 'Option Editor';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__quick_browse');

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
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);


    /**
      @todo: figure out how to save this!
      I think I need to write a storage backend but there's currently no
      documentation on how to do this. The validate method below gets called on the entity
      edit form but the submit doesn't get called on either the entity or field edit form.

    // Make this field show a text area where options global to the field can be added.
    // We need one text area per browser type... and thus first, we need the types.
    $types = local__quick_browse_get_browser_types($this->instance['settings']);

    // Current options.
    $current_values = $items[0]['value'];

    // @todo only show if they want options per page ;-)

    // Check if we're on the entity edit form or the edit field form.
    $on_entity_edit_form = (isset($form['#entity'])) ? TRUE : FALSE;

    // Only show the form on the entity page,
    // if the admin has chosen to allow per entity options.
    if($on_entity_edit_form AND ($this->instance['settings']['option_storage'] == 'trpfancy_browse_options')) {
      //return $widget;
    }

    // If there are no types yet... tell the admin what to do.
    if (empty($types)) {
      $widget['msg'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="messages warning">'
          .'You first need to set the <strong>Browser Types</strong> above, before you can set the default options for them.<br />'
          .'<strong>NOTE: You need to save the browser types first and then re-edit this field to set the options.</strong>'
          .'</div>'
      );
    }
    else {

      $widget['value'] = array(
        '#type' => 'fieldset',
        '#title' => '<em>'.$this->instance['label'].'</em> Options',
        '#tree' => TRUE,
        '#description' => 'Each section below corresponds to an option in the <em>'.$this->instance['label'].'</em> Browse drop-down. The options you provide will be used to create links for the user to browse.',
      );

      if (!$on_entity_edit_form) {
        $widget['value']['msg'] = array(
          '#type' => 'markup',
          '#markup' => '<div class="messages status">'
            .'The options you set below will become the default for all entities of this type. If you have selected "Dependant on the page shown" as the "option Source" above, then these options can be changed on the edit page.'
            .'</div>',
          '#weight' => -100,
        );
      }

      foreach ($types as $machine_name => $label) {
        $widget['value'][$machine_name] = array(
          '#type' => 'textarea',
          '#title' => '<em>' . $label . '</em> Options',
          '#description' => 'Enter the list of options you want to appear when the user selects "Browse <em>'.$label.'</em>". <strong>Each Option should be indicated on a seperate line using the key=value format</strong>
            where the key is the value saved by the module and the value is the option shown to the user.',
        );

        if (isset($current_values[$machine_name])) {
          $default_value = '';
          foreach ($current_values[$machine_name] as $k => $v) {
            $default_value .= "$k=$v\n";
          }
           $widget['value'][$machine_name]['#value'] = $default_value;
        }
      }

      $widget['value']['note'] = array(
        '#type' => 'markup',
        '#markup' => '<strong>NOTE: It may be more efficient or perferrable to set the options via an SQL query
          (advanced users only). For options global to the field, insert into the <em>trpfancy_browse_options</em> table where the <em>instance_id='.$this->instance['id'].'</em>.
          For options specific to each entity, insert into the <em>trpfancy_browse_options_per_entity</em> table where the <em>instance_id='.$this->instance['id'].'</em>.</strong>',
        '#weight' => 100,
      );
    }
    */
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
  public function validate($element, $form, &$form_state, $langcode, $delta) { }

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
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) { }

}
