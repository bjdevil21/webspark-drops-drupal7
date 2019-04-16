<?php

/**
 * Returns data from Peoplesoft for submission to What If audit.
 *
 * @return array
 */


/**
 * Start running a What If audit based on student data.
 *
 * @param $emplid
 * @param $dprog
 * @return bool
 */
function asu_comrf_start_what_if_audit($emplid, $program_plan) {
  // Send data to start automatic WIA
  // JIRA DARS-599 -- Sample URL: https://webapp4-dev.asu.edu/asudars7/rest/UserInfoService/whatif/1205472303&ESBMEBSE
  // Determine if dev or prod settings will be used
  $asu_comrf_what_if_audit_api_env = variable_get('asu_comrf_what_if_audit_api_env');
  if ($asu_comrf_what_if_audit_api_env == 'dev') {
    $what_if_audit_base_url = 'https://' . variable_get('asu_comrf_what_if_audit_api_url_dev');
  } else if ($asu_comrf_what_if_audit_api_env == 'prod') {
    $what_if_audit_base_url = 'https://' . variable_get('asu_comrf_what_if_audit_api_url_prod');
  }

  $what_if_audit_url = $what_if_audit_base_url . $emplid . '&' . $program_plan;
  $request_options = array(
    'headers' => array(),
    'method' => 'GET',
    'timeout' => 15,
  );

  // Execute call for What If audit and store results in a variable
  $result = drupal_http_request($what_if_audit_url, $request_options);
  $data = $result->data;

  // Do conversion magic to get the returned What If audit response code into object form instead of XML
  $xml = simplexml_load_string($data);
  $xml_data = $xml->ERROR->CODE;
  $json = json_encode($xml_data);
  $what_if_audit_response_obj = json_decode($json);
  $obj_index = 0;
  $what_if_audit_response_obj_code = $what_if_audit_response_obj->$obj_index;

  if ($what_if_audit_response_obj_code == 400) {
    return FALSE;
  } else if ($what_if_audit_response_obj_code == 200) {
    return TRUE;
  }
}

/**
 * Returns email from Peoplesoft for Salesforce data submission
 *
 * @param $program_plan
 * @return string
 */
function asu_comrf_get_salesforce_email($program_plan) {
  // Determine if dev or prod settings will be used
  $asu_comrf_change_major_api_email_env = variable_get('asu_comrf_change_major_api_email_env');
  if ($asu_comrf_change_major_api_email_env == 'dev') {
    $peoplesoft_data_base_url = 'https://' . variable_get('asu_comrf_mulesoft_api_client_id') . ':' . variable_get('asu_comrf_mulesoft_api_client_secret') . '@' . variable_get('asu_comrf_change_major_api_email_url_dev');
  } else if ($asu_comrf_change_major_api_email_env == 'prod') {
    $peoplesoft_data_base_url = 'https://' . variable_get('asu_comrf_mulesoft_api_client_id') . ':' . variable_get('asu_comrf_mulesoft_api_client_secret') . '@' . variable_get('asu_comrf_change_major_api_email_url_prod');
  }

  // Get email for Salesforce from PeopleSoft
  $peoplesoft_data_url = $peoplesoft_data_base_url . '?plancode=' . $program_plan;
  $request_options = array(
    'headers' => array(
      'Authorization' => "Basic OWJlMmJkYmVmMzhkNDk4NDhlNzQ2OTVkMjA2Nzk4NDE6ODhmY0U4NjY5Yjk4NDhDOThlY0QyNThkYWQxNjE5ODQ=",
    ),
    'method' => 'GET',
    'timeout' => 15,
  );
  $result = drupal_http_request($peoplesoft_data_url, $request_options);
  if (!empty($result)) {
    $program_info_raw = json_decode($result->data)->changeMajor;
    if (isset($program_info_raw->email)) {
      $salesforce_email = $program_info_raw->email;
    }
  } else {
    $salesforce_email = '';
  }

  return $salesforce_email;
}

/**
 * Submit form data to Salesforce via email.
 *
 * @param $email_to
 * @param $email_from
 * @param $asurite_id
 * @param $form
 * @param $form_state
 * @return bool
 */
function asu_comrf_salesforce_submission($email_to, $email_from, $asurite_id, $form, &$form_state) {

  $asurite_id_title = 'Request from: ';

  $major_to_change_key = $form_state['values']['asu_comrf_major_to_change'];
  $major_to_change = $form_state['complete form']['asu_comrf_major_to_change']['#options'][$major_to_change_key];
  $major_to_change_title = $form_state['complete form']['asu_comrf_major_to_change']['#title'];

  $new_major_choice_key = $form_state['values']['asu_comrf_new_major_choice'];
  $new_major_choice = $form_state['complete form']['asu_comrf_new_major_choice']['#options'][$new_major_choice_key];
  $new_major_choice_title = $form_state['complete form']['asu_comrf_new_major_choice']['#title'];

  $new_campus_choice_key = $form_state['values']['asu_comrf_new_campus_choice'];
  $new_campus_choice = $form_state['complete form']['asu_comrf_new_campus_choice']['#options'][$new_campus_choice_key];
  $new_campus_choice_title = $form_state['complete form']['asu_comrf_new_campus_choice']['#title'];

  $com_requirements_text = $form_state['complete form']['asu_comrf_com_requirements_text']['#markup'];
  $com_requirements_text_title = $form_state['complete form']['asu_comrf_com_requirements_text']['#title'];

  $requirements_check_key = $form_state['values']['asu_comrf_requirements_check'];
  $requirements_check = $form_state['complete form']['asu_comrf_requirements_check']['#options'][$requirements_check_key];
  $requirements_check_title = $form_state['complete form']['asu_comrf_requirements_check']['#title'];

  $double_check_key = $form_state['values']['asu_comrf_double_check'];
  $double_check = $form_state['complete form']['asu_comrf_double_check']['#options'][$double_check_key];
  $double_check_title = $form_state['complete form']['asu_comrf_double_check']['#title'];

  $contact_phone_number = $form_state['values']['asu_comrf_contact_phone_number'];
  $contact_phone_number_title = $form_state['complete form']['asu_comrf_contact_phone_number']['#title'];

  $major_change_source_key = $form_state['values']['asu_comrf_major_change_source'];
  $major_change_source = $form_state['complete form']['asu_comrf_major_change_source']['#options'][$major_change_source_key];
  $major_change_source_title = $form_state['complete form']['asu_comrf_major_change_source']['#title'];
  $major_change_source_other = isset($form_state['values']['asu_comrf_major_change_source_other']) ? $form_state['values']['asu_comrf_major_change_source_other'] : NULL;

  $why_change_key = $form_state['values']['asu_comrf_why_change'];
  $why_change = $form_state['complete form']['asu_comrf_why_change']['#options'][$why_change_key];
  $why_change_title = $form_state['complete form']['asu_comrf_why_change']['#title'];
  $why_change_other = isset($form_state['values']['asu_comrf_why_change_other']) ? $form_state['values']['asu_comrf_why_change_other'] : NULL;

  $html_for_body = '<p><strong>' . $asurite_id_title . '</strong><br>' . $asurite_id . '</p><br><br>' .
                   '<p><strong>' . $major_to_change_title . '</strong><br>' . $major_to_change . '</p><br><br>' .
                   '<p><strong>' . $new_major_choice_title . '</strong><br>' . $new_major_choice . '</p><br><br>' .
                   '<p><strong>' . $new_campus_choice_title . '</strong><br>' . $new_campus_choice . '</p><br><br>' .
                   '<p><strong>' . $com_requirements_text_title . '</strong><br>' . $com_requirements_text . '</p><br><br>' .
                   '<p><strong>' . $requirements_check_title . '</strong><br>' . $requirements_check . '</p><br><br>' .
                   '<p><strong>' . $double_check_title . '</strong><br>' . $double_check . '</p><br><br>' .
                   '<p><strong>' . $contact_phone_number_title . '</strong><br>' . $contact_phone_number . '</p><br><br>';

  $html_for_body .= '<p><strong>' . $major_change_source_title . '</strong><br>' . $major_change_source . '</p><br><br>';

  if ($major_change_source_other !== '') {
    $major_change_source_other_title = $form_state['complete form']['asu_comrf_major_change_source_other']['#title'];
    $html_for_body .= '<p><strong>' . $major_change_source_other_title . '</strong><br>' . $major_change_source_other . '</p><br><br>';
  }

  $html_for_body .= '<p><strong>' . $why_change_title . '</strong><br>' . $why_change . '</p>';

  if ($why_change_other !== '') {
    $why_change_other_title = $form_state['complete form']['asu_comrf_why_change_other']['#title'];
    $html_for_body .= '<br><br><p><strong>' . $why_change_other_title . '</strong><br>' . $why_change_other . '</p>';
  }

  // Compose email submission for Salesforce
  $email_params = array(
    'from' => $email_from,
    'subject' => 'Change of Major Request Form Submission',
    'body' => $html_for_body,
  );

  $sf_email = drupal_mail('asu_comrf', 'asu_comrf_sf_email', $email_to, language_default(), $email_params);

  if ($sf_email['result']) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * Save form data to the website's database.
 *
 * @param $asurite_id
 * @param $form_state
 * @param $submit_key
 * @return bool
 */
function asu_comrf_save_form_data($asurite_id, &$form_state, $submit_key) {
  // Initialize check variables
  $data_stored_successfully = FALSE;
  $stored_data = '';

  // Gather form data to store in database
  $major_to_change_key = $form_state['values']['asu_comrf_major_to_change'];
  $major_to_change = $form_state['complete form']['asu_comrf_major_to_change']['#options'][$major_to_change_key];
  $new_major_choice_key = $form_state['values']['asu_comrf_new_major_choice'];
  $new_major_choice = $form_state['complete form']['asu_comrf_new_major_choice']['#options'][$new_major_choice_key];
  $com_requirements_text = $form_state['complete form']['asu_comrf_com_requirements_text']['#markup'];

  // Decide whether the form should be submitted as a normal submission, or a WPC redirect submission
  switch ($submit_key) {
    case 'eAdvisor':
      $new_campus_choice_key = $form_state['values']['asu_comrf_new_campus_choice'];
      $new_campus_choice = $form_state['complete form']['asu_comrf_new_campus_choice']['#options'][$new_campus_choice_key];
      $requirements_check_key = $form_state['values']['asu_comrf_requirements_check'];
      $requirements_check = $form_state['complete form']['asu_comrf_requirements_check']['#options'][$requirements_check_key];
      $double_check_key = $form_state['values']['asu_comrf_double_check'];
      $double_check = $form_state['complete form']['asu_comrf_double_check']['#options'][$double_check_key];
      $contact_phone_number = $form_state['values']['asu_comrf_contact_phone_number'];
      $major_change_source_key = $form_state['values']['asu_comrf_major_change_source'];
      $major_change_source = $form_state['complete form']['asu_comrf_major_change_source']['#options'][$major_change_source_key];
      $major_change_source_other = isset($form_state['values']['asu_comrf_major_change_source_other']) ? $form_state['values']['asu_comrf_major_change_source_other'] : NULL;
      $why_change_key = $form_state['values']['asu_comrf_why_change'];
      $why_change = $form_state['complete form']['asu_comrf_why_change']['#options'][$why_change_key];
      $why_change_other = isset($form_state['values']['asu_comrf_why_change_other']) ? $form_state['values']['asu_comrf_why_change_other'] : NULL;

      // Insert data from form to database
      if ($form_state) {
        $stored_data = db_insert('asu_comrf_form_submission')
          ->fields(array(
            'asurite_id' => $asurite_id,
            'major_to_change' => $major_to_change,
            'new_major_choice' => $new_major_choice,
            'new_campus_choice' => $new_campus_choice,
            'com_requirements_text' => $com_requirements_text,
            'requirements_check' => $requirements_check,
            'double_check' => $double_check,
            'contact_phone_number' => $contact_phone_number,
            'major_change_source' => $major_change_source,
            'major_change_source_other' => $major_change_source_other,
            'why_change' => $why_change,
            'why_change_other' => $why_change_other,
            'timestamp' => date('U'),
          ))
          ->execute();
      }
      break;
    case 'WPC':
      // Insert data from form to database
      if ($form_state) {
        $stored_data = db_insert('asu_comrf_form_submission')
          ->fields(array(
            'asurite_id' => $asurite_id,
            'major_to_change' => $major_to_change,
            'new_major_choice' => $new_major_choice,
            'new_campus_choice' => $submit_key,
            'com_requirements_text' => $com_requirements_text,
            'requirements_check' => $submit_key,
            'double_check' => $submit_key,
            'contact_phone_number' => $submit_key,
            'major_change_source' => $submit_key,
            'major_change_source_other' => $submit_key,
            'why_change' => $submit_key,
            'why_change_other' => $submit_key,
            'timestamp' => date('U'),
          ))
          ->execute();
      }
      break;
  }

  // If inserting data was successful, set check variable to true
  if ($stored_data !== NULL) {
    $data_stored_successfully = TRUE;
  }

  return $data_stored_successfully;
}
