<?php

/**
 * Implements hook_views_default_views().
 */
function asu_comrf_views_default_views() {

  $view = new view();
  $view->name = 'asu_comrf_submissions';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'asu_comrf_form_submission';
  $view->human_name = 'ASU Change of Major Request Form Submissions';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'ASU Change of Major Request Form Submissions';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'sid' => 'sid',
    'asurite_id' => 'asurite_id',
    'major_to_change' => 'major_to_change',
    'new_major_choice' => 'new_major_choice',
    'new_campus_choice' => 'new_campus_choice',
    'com_requirements_text' => 'com_requirements_text',
    'requirements_check' => 'requirements_check',
    'double_check' => 'double_check',
    'contact_phone_number' => 'contact_phone_number',
    'major_change_source' => 'major_change_source',
    'major_change_source_other' => 'major_change_source_other',
    'why_change' => 'why_change',
    'why_change_other' => 'why_change_other',
    'timestamp' => 'timestamp',
  );
  $handler->display->display_options['style_options']['default'] = 'sid';
  $handler->display->display_options['style_options']['info'] = array(
    'sid' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'asurite_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'major_to_change' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'new_major_choice' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'new_campus_choice' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'com_requirements_text' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'requirements_check' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'double_check' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'contact_phone_number' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'major_change_source' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'major_change_source_other' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 1,
    ),
    'why_change' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'why_change_other' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 1,
    ),
    'timestamp' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: ASU Change of Major Request Form: sid */
  $handler->display->display_options['fields']['sid']['id'] = 'sid';
  $handler->display->display_options['fields']['sid']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['sid']['field'] = 'sid';
  /* Field: ASU Change of Major Request Form: ASURITE ID */
  $handler->display->display_options['fields']['asurite_id']['id'] = 'asurite_id';
  $handler->display->display_options['fields']['asurite_id']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['asurite_id']['field'] = 'asurite_id';
  /* Field: ASU Change of Major Request Form: Major to change */
  $handler->display->display_options['fields']['major_to_change']['id'] = 'major_to_change';
  $handler->display->display_options['fields']['major_to_change']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['major_to_change']['field'] = 'major_to_change';
  /* Field: ASU Change of Major Request Form: New major choice */
  $handler->display->display_options['fields']['new_major_choice']['id'] = 'new_major_choice';
  $handler->display->display_options['fields']['new_major_choice']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['new_major_choice']['field'] = 'new_major_choice';
  /* Field: ASU Change of Major Request Form: New campus choice */
  $handler->display->display_options['fields']['new_campus_choice']['id'] = 'new_campus_choice';
  $handler->display->display_options['fields']['new_campus_choice']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['new_campus_choice']['field'] = 'new_campus_choice';
  /* Field: ASU Change of Major Request Form: Requirements */
  $handler->display->display_options['fields']['com_requirements_text']['id'] = 'com_requirements_text';
  $handler->display->display_options['fields']['com_requirements_text']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['com_requirements_text']['field'] = 'com_requirements_text';
  $handler->display->display_options['fields']['com_requirements_text']['exclude'] = TRUE;
  /* Field: ASU Change of Major Request Form: Requirements check */
  $handler->display->display_options['fields']['requirements_check']['id'] = 'requirements_check';
  $handler->display->display_options['fields']['requirements_check']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['requirements_check']['field'] = 'requirements_check';
  /* Field: ASU Change of Major Request Form: Requirements double check */
  $handler->display->display_options['fields']['double_check']['id'] = 'double_check';
  $handler->display->display_options['fields']['double_check']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['double_check']['field'] = 'double_check';
  /* Field: ASU Change of Major Request Form: Contact phone number */
  $handler->display->display_options['fields']['contact_phone_number']['id'] = 'contact_phone_number';
  $handler->display->display_options['fields']['contact_phone_number']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['contact_phone_number']['field'] = 'contact_phone_number';
  /* Field: ASU Change of Major Request Form: Major change source */
  $handler->display->display_options['fields']['major_change_source']['id'] = 'major_change_source';
  $handler->display->display_options['fields']['major_change_source']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['major_change_source']['field'] = 'major_change_source';
  /* Field: ASU Change of Major Request Form: Major change source (other) */
  $handler->display->display_options['fields']['major_change_source_other']['id'] = 'major_change_source_other';
  $handler->display->display_options['fields']['major_change_source_other']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['major_change_source_other']['field'] = 'major_change_source_other';
  /* Field: ASU Change of Major Request Form: Why change */
  $handler->display->display_options['fields']['why_change']['id'] = 'why_change';
  $handler->display->display_options['fields']['why_change']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['why_change']['field'] = 'why_change';
  /* Field: ASU Change of Major Request Form: Why change (other) */
  $handler->display->display_options['fields']['why_change_other']['id'] = 'why_change_other';
  $handler->display->display_options['fields']['why_change_other']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['why_change_other']['field'] = 'why_change_other';
  /* Field: ASU Change of Major Request Form: Timestamp */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'asu_comrf_form_submission';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'short';
  $handler->display->display_options['fields']['timestamp']['second_date_format'] = 'panopoly_time';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/config/development/asu-comrf/submissions';

  $views[$view->name] = $view;
  return $views;
}