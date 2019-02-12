<?php

/**
 * @file
 * Handles display/submission for associating an EAD with an existing object.
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
    '#markup' => t('Select the object to link to !level. Leave blank to remove current association.', array(
      '!level' => $level_info,
    )),
  );
  // Pre-populate with the existing association if it exists.
  $association = islandora_ead_retrieve_refid_association($ead->id, $refid);
  $form_state['old_assoc_id'] = $association;

  $form['associated_object'] = array(
    'associated_object' => array(
      '#autocomplete_path' => "islandora/object/$ead->id/manage/ead/get_potential_associations",
      '#default_value' => $association,
      '#type' => 'textfield',
      '#title' => t('Select an object'),
    ),
    'actions' => array(
      '#type' => 'actions',
      'link_object' => array(
        '#type' => 'submit',
        '#value' => t('Link Object'),
      ),
      'cancel' => array(
        '#markup' => l(t('Cancel'), format_string('islandora/object/!object_id/manage/ead', array(
          '!object_id' => $ead->id,
        ))),
      ),

    ),
  );

  return $form;
}

/**
 * Submit handler; save new association relationship and/or remove existing.
 */
function islandora_ead_association_existing_object_form_submit(array $form, array &$form_state) {
  $eadid = $form_state['ead']->id;
  $refid = $form_state['refid'];
  $old_assoc_id = $form_state['old_assoc_id'];
  $new_assoc_id = $form_state['values']['associated_object'];

  if ($old_assoc_id != $new_assoc_id) {
    if ($old_assoc_id) {
      $old_assoc_object = islandora_object_load($old_assoc_id);
      $old_assoc_object->relationships->remove(FEDORA_RELS_EXT_URI, 'isPartOf', $eadid);
      $old_assoc_object->relationships->remove(FEDORA_RELS_EXT_URI, 'eadLevelId', $refid);
      drupal_set_message(t("Removed old object association."));
    }
    if ($new_assoc_id) {
      $new_assoc_object = islandora_object_load($new_assoc_id);
      $new_assoc_object->relationships->add(FEDORA_RELS_EXT_URI, 'isPartOf', $eadid);
      $new_assoc_object->relationships->add(FEDORA_RELS_EXT_URI, 'eadLevelId', $refid);
      drupal_set_message(t("Added new object association."));
    }
  }

  // Redirect browser to the new object.
  if ($new_assoc_id) {
    $form_state['redirect'] = "islandora/object/{$new_assoc_id}";
  }
  else {
    $form_state['redirect'] = "islandora/object/{$eadid}";
  }
}
