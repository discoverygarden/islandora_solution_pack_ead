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

  $label_field = variable_get('islandora_solr_object_label_field', 'fgs_label_s');

  // Make the query.
  $qp = new IslandoraSolrQueryProcessor();
  $query = islandora_solr_lesser_escape("$search_value*");
  $qp->buildQuery($query);
  $qp->solrParams['fl'] = "$label_field,PID";
  $qp->solrParams['sort'] = "PID asc";
  $qp->executeQuery(FALSE);

  // Helper function to construct labels.
  $construct_label = function ($data) use ($label_field) {
    $label = isset($data['solr_doc'][$label_field]) ? $data['solr_doc'][$label_field] : '';
    return t('@label (@pid)', array(
      '@label' => $label,
      '@pid' => $data['solr_doc']['PID'],
    ));
  };

  $results = array();
  foreach ($qp->islandoraSolrResult['response']['objects'] as $result) {
    $results[$result['PID']] = $construct_label($result);
  }
  drupal_json_output($results);
  drupal_exit();
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
  $qp = new IslandoraSolrQueryProcessor();
  $qp->solrParams['fl'] = 'PID';
  $qp->solrParams['fq'] = array(
    "RELS_EXT_isPartOf_uri_ms:\"info:fedora/{$eadid}\"",
    "RELS_EXT_eadLevelId_uri_ms:\"info:fedora/{$refid}\"",
  );
  $qp->executeQuery(FALSE);

  $result = $qp->islandoraSolrResult['response']['objects'];
  return isset($result[0]['PID']) ? $result[0]['PID'] : NULL;
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
  $ead_xml = islandora_ead_transform_with_saxonb(
    'getEadLevels.xsl',
    $ead['EAD']->content
  );
  $ead_dom = new DomDocument();
  $ead_dom->loadXML($ead_xml);

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

/**
 * Helper to transform XML using the named XSLT with saxonb.
 *
 * @param string $xsl_name
 *   Name of the XSL file to be used, relative to this module's "xml"
 *   directory.
 * @param string $doc
 *   The XML to convert.
 * @param array $params
 *   An optional associative array of XSLT parameters mapping parameter names
 *   to their values.
 *
 * @return string
 *   The transformed xml.
 */
function islandora_ead_transform_with_saxonb($xsl_name, $doc, array $params = array()) {
  $module_path = drupal_get_path('module', 'islandora_ead');
  $xsl_path = format_string('!path/xml/!xsl', array(
    '!path' => $module_path,
    '!xsl' => $xsl_name,
  ));

  $pipes = array();
  $equal_join_and_escape = function ($key, $value) {
    return escapeshellarg("$key=$value");
  };
  $escaped = array_map($equal_join_and_escape, array_keys($params), $params);
  $param_string = implode(' ', $escaped);

  $process = proc_open("saxonb-xslt -versionmsg:off - $xsl_path $param_string",
    array(
      0 => array(
        'pipe',
        'r',
      ),
      1 => array(
        'pipe',
        'w',
      ),
      2 => array(
        'file',
        '/dev/null',
        'a',
      ),
    ),
    $pipes
  );

  fwrite($pipes[0], $doc);
  fclose($pipes[0]);
  $saxonb_transformed = stream_get_contents($pipes[1]);
  fclose($pipes[1]);
  proc_close($process);

  watchdog('islandora_ead', $saxonb_transformed);
  return $saxonb_transformed;
}
