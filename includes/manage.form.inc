<?php

/**
 * @file
 * Handles displaying/managing level information for EAD content inventories.
 *
 * This file is part of the Islandora EAD Solution Pack.
 * Copyright (C) 2015  Drexel University.
 *
 * The Islandora EAD Solution Pack is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * The Islandora EAD Solution Pack is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with The Islandora EAD Solution Pack; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * Defines a form for viewing EAD content inventory information.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 * @param AbstractObject $object
 *   The object representing the EAD.
 */
function islandora_ead_manage_form(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'dgi_saxon_helper', 'includes/utilities');
  module_load_include('inc', 'islandora_ead', 'includes/utilities');

  $header = array(
    t('Level'),
    t('Title'),
    t('Extent'),
    t('Description'),
    t('ID'),
    t('Containers'),
    t('Association'),
  );

  $xslt_path = drupal_get_path('module', 'islandora_ead') . '/xml/getEadLevels.xsl';
  $ead_xml = dgi_saxon_helper_transform_string_to_string($xslt_path, $object['EAD']->content);
  $ead_dom = new DomDocument();
  $ead_dom->loadXML($ead_xml);

  $partsxml = new DOMDocument();
  $partsxml->loadXML($ead_dom->saveXML());
  $partsxp = new DOMXPath($partsxml);
  $entities = $partsxp->query('/entities/entity');

  $limit = 20;
  pager_default_initialize($entities->length, $limit);
  $page = pager_find_page();

  $parts = array();
  $associations = islandora_ead_retrieve_associations($object->id);
  // Little bit of optimization; DOMNodeList::length is computed, so let's do
  // that once.
  $entities_length = $entities->length;
  $page_start = $limit * $page;
  for ($idx = $page_start; $idx < min($limit + $page_start, $entities_length); $idx++) {
    $parts[$idx] = _islandora_ead_get_entity_column_info($object->id, $entities->item($idx), $partsxp, $associations);
  }

  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $parts,
    '#empty' => 'Empty Finding Aid',
  );
  $form['pager'] = array('#theme' => 'pager');

  return $form;
}

/**
 * Helper function to get a formatted array of entity info for use in the table.
 *
 * @param string $pid
 *   The PID of the object this column info is being generated for.
 * @param DOMNode $entity
 *   The entity node to get information from.
 * @param DOMXPath $dxp
 *   A DOMXPath object for the entity XML.
 * @param array $associations
 *   Array representing associations for the object from which this entity was
 *   sourced, as returned by islandora_ead_retrieve_associations().
 *
 * @return array
 *   An array of column information for the table row for this entity.
 */
function _islandora_ead_get_entity_column_info($pid, DOMNode $entity, DOMXPath $dxp, array $associations) {
  $first_or_null = function ($val, $relative) use ($dxp) {
    $list = $dxp->query($val, $relative);
    if (!$list->length) {
      return NULL;
    }
    return (string) trim($list->item(0)->textContent);
  };
  $id = $first_or_null('id', $entity);
  $info = array(
    'level' => $first_or_null('level', $entity),
    'title' => $first_or_null('title', $entity),
    'extent' => $first_or_null('extent', $entity),
    'desc' => $first_or_null('desc', $entity),
    'id' => $id,
  );
  $containers = array();
  foreach ($dxp->query('container', $entity) as $container) {
    $container_bits = array(
      'type' => $first_or_null('type', $container),
      'value' => $first_or_null('value', $container),
    );
    if ($container_bits['type']) {
      $containers[] = "{$container_bits['type']}: {$container_bits['value']}";
    }
  }
  $info[] = implode(', ', $containers);

  $association_form_url = format_string('islandora/object/!pid/manage/ead/!refid', array(
    '!pid' => $pid,
    '!refid' => $id,
  ));
  if (array_key_exists($id, $associations)) {
    $association_form_url .= '/existing';
  }
  $info[] = l(t('Manage Association'), $association_form_url);
  return $info;
}
