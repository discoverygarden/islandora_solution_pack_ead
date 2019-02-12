<?php

/**
 * @file
 * Utility functions.
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
 * Autocomplete callback; return all objects that can be an association.
 */
function islandora_ead_get_potential_associations($search_value) {
  $tuque = islandora_get_tuque_connection();
  $query = <<<EOQ
SELECT ?pid ?label
FROM <#ri>
WHERE {
  ?pid <fedora-model:label> ?label
  FILTER(regex(?label, '$search_value', 'i') || regex(str(?pid), '$search_value', 'i'))
}
EOQ;
  $results = $tuque->repository->ri->sparqlQuery($query);

  $return = array();
  foreach ($results as $objects) {
    $pid = $objects['pid']['value'];
    if (islandora_namespace_accessible($pid)) {
      $return[$pid] = t('@label (@pid)', array(
        '@label' => $objects['label']['value'],
        '@pid' => $pid,
      ));
    }
  }
  drupal_json_output($return);
}

/**
 * Return mapping of internal inventory level IDs to PIDs of associated objects.
 *
 * @param string $eadid
 *   The PID of the EAD object to retrieve the associations of.
 */
function islandora_ead_retrieve_associations($eadid) {

  $tuque = islandora_get_tuque_connection();
  $sparql_query = <<<EOQ
SELECT ?partid ?levelid
FROM <#ri>
WHERE {
  ?partid <fedora-rels-ext:isPartOf> <info:fedora/$eadid> ;
          <fedora-rels-ext:eadLevelId> ?levelid .
}
EOQ;

  $sparql_results = $tuque->repository->ri->sparqlQuery($sparql_query);
  $associations = array();
  foreach ($sparql_results as $objects) {
    $associations[$objects['levelid']['value']] = $objects['partid']['value'];
  }

  return $associations;
}

/**
 * Get the associated object for the given EAD object and refid.
 *
 * @param string $eadid
 *   The PID of the EAD object to retrieve the association of.
 * @param string $refid
 *   The level ID to retrieve the association of.
 *
 * @return string
 *   The PID of the associated object or NULL.
 */
function islandora_ead_retrieve_refid_association($eadid, $refid) {
  $tuque = islandora_get_tuque_connection();
  $sparql_query = <<<EOQ
SELECT ?partid
FROM <#ri>
WHERE {
  ?partid <fedora-rels-ext:isPartOf> <info:fedora/$eadid> ;
          <fedora-rels-ext:eadLevelId> <info:fedora/$refid> .
}
EOQ;

  $sparql_results = $tuque->repository->ri->sparqlQuery($sparql_query);
  return $sparql_results ? $sparql_results[0]['partid']['value'] : NULL;
}

/**
 * Get the level information for the given EAD object and refid.
 *
 * @param AbstractObject $ead
 *   The EAD object to retrieve the association of.
 * @param string $refid
 *   The level ID to retrieve the association of.
 *
 * @return string
 *   String describing the level information.
 */
function islandora_ead_get_level_info(AbstractObject $ead, $refid) {
  $xsl = new DOMDocument();
  $xsl->load(drupal_get_path('module', 'islandora_ead') . '/xml/getEadLevels.xsl');

  $xslt = new XSLTProcessor();
  $xslt->importStyleSheet($xsl);

  $ead_dom = new DOMDocument();
  $ead_dom->loadXML($ead['EAD']->content);
  $ead_dom = $xslt->transformToDoc($ead_dom);

  $xpath = new DOMXPath($ead_dom);
  $entity = $xpath->query("/entities/entity[id = '$refid']")->item(0);
  $containers = array();
  foreach ($xpath->query('container', $entity) as $container) {
    $type = $xpath->evaluate('string(type)', $container);
    $value = $xpath->evaluate('string(value)', $container);
    if ($type && $value) {
      $containers[] = format_string("!type: !value", array(
        '!type' => $type,
        '!value' => $value,
      ));
    }
  }
  $level_info = implode(', ', $containers);
  return $level_info;
}
