<?php

/**
 * @file
 * webspark_extras.features.content_menu_links.inc
 */

/**
 * Implements hook_content_menu_links_defaults().
 */
function webspark_extras_content_menu_links_defaults() {
  $menu_links = array();

  // Exported menu link: management:admin/help
  $menu_links['management:admin/help'] = array(
    'menu_name' => 'management',
    'link_path' => 'admin/field/edit/%/%/%',
    'router_path' => 'admin/field/edit/%/%/%',
    'link_title' => '',
    'options' => array(),
    'module' => 'system',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 0,
    'parent_path' => 'admin',
  );
  // Exported menu link: management:https
  $menu_links['management:https'] = array(
    'menu_name' => 'management',
    'link_path' => 'admin/field/edit/%/%/%',
    'router_path' => 'admin/field/edit/%/%/%',
    'link_title' => '',
    'options' => array(),
    'module' => 'system',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 0,
    'parent_path' => 'admin',
  );
  // Exported menu link: management_help:webspark-help
  $menu_links['management_help:webspark-help'] = array(
    'menu_name' => 'management',
    'link_path' => 'admin/help',
    'router_path' => 'admin/help',
    'link_title' => 'Help from...',
    'options' => array(
      'attributes' => array(
        'title' => 'Drupal help from this site.',
      ),
      'identifier' => 'management_help:webspark-help',
    ),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 1,
    'expanded' => 0,
    'weight' => -42,
    'parent_path' => 'admin',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Help from...');


  return $menu_links;
}
