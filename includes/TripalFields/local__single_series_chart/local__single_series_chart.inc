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

    $element['data_options']['data_msg'] = [
      '#type' => 'item',
      '#markup' => 'Each series of data in this chart consists of multiple "categories" where each category has a label and a count. For example, a chart showing the types of features for a given organism would have labels such as "gene", "genetic marker", "SNP" and the counts would indicate the number of genes, genetic markers and SNPs available for the given organism.',
    ];

    // The column whose value should be the category labels on the chart.
    $element['data_options']['label_column'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Label Column',
      '#description' => 'The column from the materialized view whose value should be the category labels within a single series of data.',
      '#default_value' => $settings['data_options']['label_column'],
    ];

    // The column containing the numbers determining the category size.
    $element['data_options']['count_column'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Count Column',
      '#description' => 'The column from the materialized view whose value should be the category count within a single series of data.',
      '#default_value' => $settings['data_options']['count_column'],
    ];

    return $element;
  }

  /**
   *
   */
  public function instanceSettingsFormValidate($form, &$form_state) {

    parent::instanceSettingsFormValidate($form, $form_state);
    $values = $form_state['values']['instance']['settings']['data_options'];

    // Check the label column exists.
    if (!chado_column_exists($values['materialized_view'], $values['label_column'])) {
      form_set_error('instance][settings][data_options][label_column',
        t('The <em>Label Column</em> must exist in the %mview materialized view',
          ['%mview' => $values['materialized_view']]));
    }

    // Check the count column exists.
    if (!chado_column_exists($values['materialized_view'], $values['count_column'])) {
      form_set_error('instance][settings][data_options][count_column',
        t('The <em>Count Column</em> must exist in the %mview materialized view',
          ['%mview' => $values['materialized_view']]));
    }
  }
}
