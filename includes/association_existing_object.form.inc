<?php

/**
 * @file
 * Handles the display/submission for associating an EAD with an existing
 * object.
 */

/**
 * Defines a file upload form for uploading an associated file.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 * @param AbstractObject $ead
 *   The object representing the EAD.
 * @param string $refid
 *   ID of the level in the EAD content inventory being modified.
 *
 * @return array
 *   The drupal form definition.
 */
function islandora_ead_association_existing_object_form(array $form, array &$form_state, AbstractObject $ead, $refid) {
  module_load_include('inc', 'islandora_ead', 'includes/utilities');
  $form_state['ead'] = $ead;
  $form_state['refid'] = $refid;

  $level_info = islandora_ead_get_level_info($ead, $refid);
  $form['description'] = array(
    '#markup' => t('Select the object to link to !level:', array(
      '!level' => $level_info,
    )),
  );
  // Pre-populate with the existing association if it exists.
  $association = islandora_ead_retrieve_refid_association($ead->id, $refid);
  $form['associated_object'] = array(
    '#autocomplete_path' => "islandora/object/$ead->id/manage/ead/get_potential_associations",
    '#default_value' => $association,
    '#required' => TRUE,
    '#type' => 'textfield',
    '#title' => t('Select an object'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Submit handler; save association relationship.
 */
function islandora_ead_association_existing_object_form_submit(array $form, array &$form_state) {
  $eadid = $form_state['ead']->id;
  $refid = $form_state['refid'];

  $associated_object = islandora_object_load($form_state['values']['associated_object']);
  $associated_object->relationships->add(FEDORA_RELS_EXT_URI, 'isPartOf', $eadid);
  $associated_object->relationships->add(FEDORA_RELS_EXT_URI, 'eadLevelId', $refid);

  // Redirect browser to the object.
  $form_state['redirect'] = "islandora/object/{$associated_object->id}";
}
