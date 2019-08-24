<?php

function asu_comrf_admin() {
  $form = array();

  // Define fieldsets and child fields for admin form fields

  // Global configuration
  $form['asu_comrf_global_config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Module Settings'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_global_config']['asu_comrf_module_state'] = array(
    '#type' => 'radios',
    '#title' => t('Change of Major module state'),
    '#description' => t('If needed, use demo mode to skip over all configuration settings listed below for basic testing of block and form field functionality. Please note, however, that demo mode is currently a pipe dream and has not been fully implemented, so it may not do anything but break stuff.'),
    '#options' => array(
      'config' => t('Configure'),
      'demo' => t('Demo'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_module_state', 'config'),
  );

  // MuleSoft API
  $form['asu_comrf_mulesoft_api'] = array(
    '#type' => 'fieldset',
    '#title' => t('MuleSoft API Settings'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_mulesoft_api_app_name'] = array(
    '#type' => 'textfield',
    '#title' => t('MuleSoft API application name'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_mulesoft_api_app_name', 'esb'),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_mulesoft_api_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('MuleSoft API client ID'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_mulesoft_api_client_id', ''),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_mulesoft_api_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('MuleSoft API client secret'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_mulesoft_api_client_secret', ''),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_change_major_api_student_env'] = array(
    '#type' => 'radios',
    '#title' => t('Student information: development or production?'),
    '#options' => array(
      'dev' => t('Development'),
      'prod' => t('Production'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_change_major_api_student_env', 'dev'),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_change_major_api_student_url_dev'] = array(
    '#type' => 'textfield',
    '#title' => t('Student information development URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_change_major_api_student_url_dev', ''),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_change_major_api_student_url_prod'] = array(
    '#type' => 'textfield',
    '#title' => t('Student information production URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_change_major_api_student_url_prod', ''),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_change_major_api_email_env'] = array(
    '#type' => 'radios',
    '#title' => t('Program email for Salesforce: development or production?'),
    '#options' => array(
      'dev' => t('Development'),
      'prod' => t('Production'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_change_major_api_email_env', 'dev'),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_change_major_api_email_url_dev'] = array(
    '#type' => 'textfield',
    '#title' => t('Program email development URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_change_major_api_email_url_dev', ''),
  );
  $form['asu_comrf_mulesoft_api']['asu_comrf_change_major_api_email_url_prod'] = array(
    '#type' => 'textfield',
    '#title' => t('Program email production URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_change_major_api_email_url_prod', ''),
  );

  // What If audit API
  $form['asu_comrf_what_if_audit_api'] = array(
    '#type' => 'fieldset',
    '#title' => t('What If Audit API Settings'),
    '#weight' => 3,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_what_if_audit_api']['asu_comrf_what_if_audit_api_state'] = array(
    '#type' => 'radios',
    '#title' => t('What If audit: enabled or disabled?'),
    '#options' => array(
      1 => t('Enabled'),
      0 => t('Disabled'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_what_if_audit_api_state', 0),
  );
  $form['asu_comrf_what_if_audit_api']['asu_comrf_what_if_audit_api_env'] = array(
    '#type' => 'radios',
    '#title' => t('What If audit: development or production?'),
    '#options' => array(
      'dev' => t('Development'),
      'prod' => t('Production'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_what_if_audit_api_env', 'dev'),
  );
  $form['asu_comrf_what_if_audit_api']['asu_comrf_what_if_audit_api_url_dev'] = array(
    '#type' => 'textfield',
    '#title' => t('What If audit development URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_what_if_audit_api_url_dev', ''),
  );
  $form['asu_comrf_what_if_audit_api']['asu_comrf_what_if_audit_api_url_prod'] = array(
    '#type' => 'textfield',
    '#title' => t('What If audit production URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_what_if_audit_api_url_prod', ''),
  );

  // Degree Search
  $form['asu_comrf_degree_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Degree Search Settings'),
    '#weight' => 4,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_env'] = array(
    '#type' => 'radios',
    '#title' => t('Degree Search: development or production?'),
    '#options' => array(
      'dev' => t('Development'),
      'prod' => t('Production'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_env', 'dev'),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_url_dev'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search development URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_url_dev', ''),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_url_prod'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search production URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_url_prod', ''),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_service_method'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service method'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_service_method', ''),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_acad_plan_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'AcadPlan\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_acad_plan_index', 0),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_degree_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'Degree\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_degree_index', 1),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_trnscr_descr_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'TrnscrDescr\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_trnscr_descr_index', 4),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_change_major_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'ChangeMajor\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_change_major_index', 3),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_trnscr_descr_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'TrnscrDescr\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_trnscr_descr_index', 4),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_diploma_descr_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'DiplomaDescr\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_diploma_descr_index', 5),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_subplans_array_result_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'MajormapSubplans\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_subplans_array_result_index', 6),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_result_campus_string_array_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree Search service call: \'CampusStringArray\' field result index'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_result_campus_string_array_index', 7),
  );
  $form['asu_comrf_degree_search']['asu_comrf_degree_search_block_markup'] = array(
    '#type' => 'textarea',
    '#title' => t('Degree Search block markup'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_degree_search_block_markup', ''),
  );

  // Form header block
  $form['asu_comrf_form_header_block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form Header Block Settings'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_form_header_block']['asu_comrf_form_header_block_markup'] = array(
    '#type' => 'textarea',
    '#title' => t('Form header markup'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_form_header_block_markup', ''),
  );

  // Student info block
  $form['asu_comrf_student_info_block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Student Information Block Settings'),
    '#weight' => 6,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_student_info_block']['asu_comrf_student_info_block_env'] = array(
    '#type' => 'radios',
    '#title' => t('Student information block: development or production?'),
    '#options' => array(
      'dev' => t('Development'),
      'prod' => t('Production'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_student_info_block_env', 'dev'),
  );

  // Form pages (form itself, confirmation page, notice page)
  $form['asu_comrf_form_pages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form Pages Settings'),
    '#weight' => 7,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_form_pages']['asu_comrf_form_confirmation_text_markup'] = array(
    '#type' => 'textarea',
    '#title' => t('Confirmation page markup'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_form_confirmation_text_markup', ''),
  );
  $form['asu_comrf_form_pages']['asu_comrf_form_notice_text_markup'] = array(
    '#type' => 'textarea',
    '#title' => t('Notice page markup'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_form_notice_text_markup', ''),
  );
  $form['asu_comrf_form_pages']['asu_comrf_form_major_change_source_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Change of Major request form: index for \'Other\' choice on \'How did you hear about this new major?\''),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_form_major_change_source_index', 8),
  );
  $form['asu_comrf_form_pages']['asu_comrf_form_why_change_index'] = array(
    '#type' => 'textfield',
    '#title' => t('Change of Major request form: index for \'Other\' choice on \'Why do you want to change your current major?\''),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_form_why_change_index', 7),
  );

  // W.P. Carey Change of Major config
  $form['asu_comrf_wpc_com'] = array(
    '#type' => 'fieldset',
    '#title' => t('W.P. Carey Change of Major Settings'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['asu_comrf_wpc_com']['asu_comrf_wpc_com_state'] = array(
    '#type' => 'radios',
    '#title' => t('W.P. Carey Change of Major customizations'),
    '#options' => array(
      0 => t('Off'),
      1 => t('On'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_wpc_com_state', 0),
  );
  $form['asu_comrf_wpc_com']['asu_comrf_wpc_com_env'] = array(
    '#type' => 'radios',
    '#title' => t('W.P. Carey Change of Major URL: development or production?'),
    '#options' => array(
      'dev' => t('Development'),
      'prod' => t('Production'),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_wpc_com_env', 'dev'),
  );
  $form['asu_comrf_wpc_com']['asu_comrf_wpc_com_url_dev'] = array(
    '#type' => 'textfield',
    '#title' => t('W.P. Carey Change of Major development URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_wpc_com_url_dev', ''),
  );
  $form['asu_comrf_wpc_com']['asu_comrf_wpc_com_url_prod'] = array(
    '#type' => 'textfield',
    '#title' => t('W.P. Carey Change of Major production URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('asu_comrf_wpc_com_url_prod', ''),
  );

  return system_settings_form($form);
}
