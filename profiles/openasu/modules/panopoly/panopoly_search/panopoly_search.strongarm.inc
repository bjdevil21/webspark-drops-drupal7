<?php
/**
 * @file
 * panopoly_search.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function panopoly_search_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'facetapi:block_cache:search_api@database_node_index';
  $strongarm->value = -1;
  $export['facetapi:block_cache:search_api@database_node_index'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'facetapi:block_cache:search_api@node_index';
  $strongarm->value = -1;
  $export['facetapi:block_cache:search_api@node_index'] = $strongarm;

  return $export;
}
