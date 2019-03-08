<?php
/**
 * @file
 * eoss_webforms_inputformats.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function eoss_webforms_inputformats_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'use text format filtered_html'.
  $permissions['use text format filtered_html'] = array(
    'name' => 'use text format filtered_html',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
    ),
    'module' => 'filter',
  );

  // Exported permission: 'use text format full'.
  $permissions['use text format full'] = array(
    'name' => 'use text format full',
    'roles' => array(
      'administrator' => 'administrator',
    ),
    'module' => 'filter',
  );

  return $permissions;
}
