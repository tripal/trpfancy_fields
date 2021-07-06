<?php
/**
 * @file 
 * This is form to configure field identifier data id prefix.
 */

namespace Drupal\trpfancy_fields\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * TripalFancyFieldsDataIdForm.
 */
class TripalFancyFieldsDataIdForm extends ConfigFormBase {
  const SETTINGS = 'tripal_fancyfields.settings';

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_fancyfields_configuration';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      static::SETTINGS,
    ];
  }

  /**
   * {@inheritdoc}
   * Build form.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    // Attach library/style.
    $form['#attached']['library'][] = 'trpfancy_fields/tripal-fancyfields-config';

    // Configuration/module variables.
    $config = $this->config(static::SETTINGS);
   
    // System variable used to store prefix value:
    // NOTE: this variable is used by data__identifier field.
    $cur_prefix = $config->get('tripal_fancyfields_identifier_config.prefix');
  
    // Show an example of the prefix in action.
    $form['preview_prefix'] = [
      '#type' => 'inline_template',
      '#template' => 'Example (Prefix + ID#): <br />
        <div id="tripal-fancyfields-box-id">' . $cur_prefix . '<span>' . mt_rand(7777, 9999) . '</span></div>',
    ];
  
    // Prefix field.
    $form['fld_text_identifier_prefix'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Prefix'),
      '#description' => $this->t('Prefix used to generate uniquename for Identifier field.'),
      '#default_value' => $cur_prefix,
      '#required' => TRUE,
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   * Validate configuration.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritdoc}
   * Save configuration.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $fld_value_prefix = $form_state->getValue('fld_text_identifier_prefix');
    
    $this->configFactory->getEditable(static::SETTINGS)
      ->set('tripal_fancyfields_identifier_config.prefix', $fld_value_prefix)
      ->save();

    return parent::submitForm($form, $form_state);
  }
}