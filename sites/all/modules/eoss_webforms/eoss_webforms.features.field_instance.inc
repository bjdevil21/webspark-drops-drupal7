<?php
/**
 * @file
 * eoss_webforms.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function eoss_webforms_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'node-webform-body'.
  $field_instances['node-webform-body'] = array(
    'bundle' => 'webform',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 0,
      ),
      'featured' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(
          'trim_length' => 600,
        ),
        'type' => 'text_summary_or_trimmed',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'body',
    'label' => 'Body',
    'required' => FALSE,
    'settings' => array(
      'display_summary' => TRUE,
      'text_processing' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'module' => 'text',
      'settings' => array(
        'rows' => 20,
        'summary_rows' => 5,
      ),
      'type' => 'text_textarea_with_summary',
      'weight' => 31,
    ),
  );

  // Exported field_instance: 'node-webform-field_microsite_menu'.
  $field_instances['node-webform-field_microsite_menu'] = array(
    'bundle' => 'webform',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 2,
      ),
      'featured' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_microsite_menu',
    'label' => 'Microsite Menu',
    'required' => 0,
    'settings' => array(
      'blockreference_modules' => array(
        'asu_brand' => 0,
        'asu_chat' => 0,
        'block' => 0,
        'cas' => 0,
        'commerce_cart' => 0,
        'devel' => 0,
        'diff' => 0,
        'easy_breadcrumb' => 0,
        'facetapi' => 0,
        'google_appliance' => 0,
        'jquery_countdown_timer' => 0,
        'masquerade' => 0,
        'mega_footer' => 0,
        'menu' => 'menu',
        'menu_block' => 0,
        'node' => 0,
        'quicktabs' => 0,
        'search' => 0,
        'shortcut' => 0,
        'system' => 0,
        'tb_megamenu' => 0,
        'twitter_block' => 0,
        'user' => 0,
        'views' => 0,
        'webform' => 0,
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => 1,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Body');
  t('Microsite Menu');

  return $field_instances;
}
