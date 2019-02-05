<?php

/**
 * @file
 * The EAD object association form.
 *
 * Presents options for associating the EAD with a new object or an existing
 * one.
 */

/**
 * EAD object association form.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 * @param AbstractObject $ead
 *   The object representing the ead.
 * @param string $refid
 *   ID of the level n the EAD content inventory being modified.
 *
 * @return array
 *   The drupal form definition.
 */
function islandora_ead_manage_association_form(array $form, array &$form_state, AbstractObject $ead, $refid) {
  module_load_include('inc', 'islandora_ead', 'includes/utilities');
  $options = array();

  $form_state['ead'] = $ead;
  $form_state['refid'] = $refid;

  $form = array(
    'description' => array(
      '#markup' => t('Manage EAD object association.'),
    ),
    'actions' => array(
      '#type' => 'actions',
      'new_object' => array(
        '#type' => 'submit',
        '#value' => t('Create New Object'),
        '#submit' => array('islandora_ead_new_object_submit'),
      ),
      'existing_object' => array(
        '#type' => 'submit',
        '#value' => t('Link to Existing Object'),
        '#submit' => array('islandora_ead_existing_object_submit'),
      ),
      'cancel' => array(
        '#markup' => l(t('Cancel'), format_string('islandora/object/!object_id', array(
          '!object_id' => $ead->id,
        ))),
      ),
    ),
  );
  return $form;
}

/**
 * Submit handler for 'create new object' EAD assocation.
 */
function islandora_ead_new_object_submit(array $form, array &$form_state) {
  $ead = $form_state['ead'];
  $refid = $form_state['refid'];
  $form_state['redirect'] = array("islandora/object/$ead/manage/ead/$refid/create");
}

/**
 * Submit handler for 'link to existing object' EAD assocation.
 */
function islandora_ead_existing_object_submit(array $form, array &$form_state) {
  $ead = $form_state['ead'];
  $refid = $form_state['refid'];
  $form_state['redirect'] = array("islandora/object/$ead/manage/ead/$refid/existing");
}
