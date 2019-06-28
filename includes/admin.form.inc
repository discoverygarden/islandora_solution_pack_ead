<?php

/**
 * @file
 * Admin settings for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form.
 */
function islandora_ead_admin(array $form, array &$form_state) {

  module_load_include('inc', 'islandora', 'includes/utilities');
  $get_default_value = function ($name, $default) use (&$form_state) {
    return isset($form_state['values'][$name]) ? $form_state['values'][$name] : variable_get($name, $default);
  };

  $saxonb = $get_default_value('islandora_ead_saxonb_path', '/usr/bin/saxonb-xslt');
  $form['islandora_ead_saxonb_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to saxonb-xslt.'),
    '#description' => t(
      'Path to the saxonb-xslt binary. For example /usr/bin/saxonb-xslt.<br>!msg',
      array('!msg' => islandora_executable_available_message($saxonb))
    ),
    '#default_value' => $saxonb,
    '#required' => TRUE,
    '#prefix' => '<div id="saxonb-wrapper">',
    '#suffix' => '</div>',
    '#ajax' => array(
      'callback' => 'islandora_ead_admin_form_saxonb_ajax_callback',
      'wrapper' => 'saxonb-wrapper',
      'effect' => 'fade',
      'event' => 'change',
    ),
  );

  return system_settings_form($form);
}

/**
 * Ajax callback for the saxonb path textfield.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The element to render as part of the ajax callback.
 */
function islandora_ead_admin_form_saxonb_ajax_callback(array $form, array $form_state) {
  return $form['islandora_ead_saxonb_path'];
}
