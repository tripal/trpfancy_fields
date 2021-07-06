<?php
/**
 * @file 
 * This is the controller for Tripal Fancy Fields configure field page.
 */

namespace Drupal\trpfancy_fields\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Defines TripalFancyFieldsConfigureController class.
 * 
 */
class TripalFancyFieldsConfigureController extends ControllerBase {  
  /**
   * Tripal Fancy Fields configure field page.
   */
  public function ui() {
    return [
      '#theme' => 'theme-tripal-fancyfields-configure',
      '#attached' => []
    ];  
  }
}