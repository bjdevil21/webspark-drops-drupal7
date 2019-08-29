<?php

/**
 * Returns data from Peoplesoft for submission to What If audit.
 *
 * @return array
 */


/**
 * Gets DPROG and associated email for Salesforce based on a plancode via MuleSoft API
 *
 * @param $program_plan
 * @return array
 */
function asu_comrf_get_program_info($program_plan) {
  $program_info = array();

  // Determine if dev or prod settings will be used and build URL
  $api_email_env = variable_get('asu_comrf_change_major_api_email_env', 'dev');
  $api_email_url = variable_get('asu_comrf_change_major_api_email_url_' . $api_email_env);
  $asu_comrf_ms_auth = variable_get('asu_comrf_ms_auth', '');

  if (!empty($api_email_url) && !empty($asu_comrf_ms_auth)) {
    $peoplesoft_data_base_url = 'https://' . variable_get('asu_comrf_mulesoft_api_client_id') . ':' . variable_get('asu_comrf_mulesoft_api_client_secret') . '@' . $api_email_url;
  }

  // Get email for Salesforce from PeopleSoft
  $peoplesoft_data_url = $peoplesoft_data_base_url . '?plancode=' . $program_plan;
  $request_options = array(
    'headers' => array(
      'Authorization' => $asu_comrf_ms_auth,
    ),
    'method' => 'GET',
  );

//  watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft URL: ' . $peoplesoft_data_url, array(), WATCHDOG_INFO);

  $result = drupal_http_request($peoplesoft_data_url, $request_options);

  // See https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html for error codes.
  if (($result->code > 0) && ($result->code < 125)) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call for program plan ' . $program_plan . ' failed with system error ' . $result->code
      . '. Check Mulesoft API to see if it is working.', array(), WATCHDOG_ERROR);
    drupal_goto("request/error");
    drupal_exit();
  } elseif (isset($result->error)) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call for program plan ' . $program_plan . ' not working. Returns error ' . check_plain($result->error),
      array(), WATCHDOG_ERROR);
    drupal_goto("request/notice");
    drupal_exit();
  }

  if (!empty($result)) {
    $program_info_raw = json_decode($result->data)->changeMajor;
    if (isset($program_info_raw)) {
      $program_info['email'] = $program_info_raw->email;
      $program_info['dprog'] = $program_info_raw->dprog;
      //watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft response', array(), WATCHDOG_INFO);
    } else {
      $program_info['email'] == 'PSNoMail@asu.edu';
      $program_info['dprog'] == 'BA ACC BS';
      watchdog('asu_comrf', 'Mulesoft response program_info_raw empty from ' . $peoplesoft_data_url . '. Using default BA ACC BS.', array(), WATCHDOG_ERROR);
    }
  } else {
    $program_info['email'] == 'PSNoMail@asu.edu';
    $program_info['dprog'] == 'BA ACC BS';
    watchdog('asu_comrf', 'Mulesoft response was empty from ' . $peoplesoft_data_url . '. Using default BA ACC BS.', array(), WATCHDOG_ERROR);
  }
  return $program_info;
}

/**
 * Start running a What If audit based on student data.
 *
 * @param $emplid
 * @param $dprog
 * @return bool
 */
function asu_comrf_start_what_if_audit($emplid, $dprog) {
  // Send data to start automatic WIA
  // JIRA DARS-599 -- Sample URL: 'https://webapp4-dev.asu.edu/asudars7/rest/UserInfoService/whatif/1205472303&ES BME BSE'
  // Determine if dev or prod settings will be used
  $asu_comrf_what_if_audit_api_env = variable_get('asu_comrf_what_if_audit_api_env');
  if ($asu_comrf_what_if_audit_api_env == 'dev') {
    $what_if_audit_base_url = 'https://' . variable_get('asu_comrf_what_if_audit_api_url_dev');
  } else if ($asu_comrf_what_if_audit_api_env == 'prod') {
    $what_if_audit_base_url = 'https://' . variable_get('asu_comrf_what_if_audit_api_url_prod');
  }

  $dprog = str_replace(' ', '%20', $dprog);

  $what_if_audit_url = $what_if_audit_base_url . $emplid . '&' . $dprog;
  $request_options = array(
    'method' => 'GET',
  );

  watchdog('asu_comrf', 'The intended What If audit URL is: ' . $what_if_audit_url, array(), WATCHDOG_INFO);

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

  watchdog('asu_comrf', 'The What If audit returned the following result code: ' . $what_if_audit_response_obj_code, array(), WATCHDOG_INFO);

  if ($what_if_audit_response_obj_code == 400) {
    return FALSE;
  } else if ($what_if_audit_response_obj_code == 200) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * Submit form data to Salesforce via email.
 *
 * @param $email_to
 * @param $email_from
 * @param $student_info
 * @param $form_state
 * @return bool
 */
function asu_comrf_salesforce_submission($email_to, $email_from, $student_info, &$form_state) {

  $student_info_title = 'Request from: ';
  $student_info_name = $student_info['name'];
  $student_info_emplid = $student_info['emplid'];

  $major_to_change_key = $form_state['values']['asu_comrf_major_to_change'];
  $major_to_change = $form_state['complete form']['asu_comrf_major_to_change']['#options'][$major_to_change_key];
  $major_to_change_title = $form_state['complete form']['asu_comrf_major_to_change']['#title'];

  $new_major_choice_key = $form_state['values']['asu_comrf_new_major_choice'];
  $new_major_choice = $form_state['complete form']['asu_comrf_new_major_choice']['#options'][$new_major_choice_key];
  $new_major_choice_title = $form_state['complete form']['asu_comrf_new_major_choice']['#title'];

  // If subplan is selected and submitted
  $new_subplan_choice_key = 'none';
  if (isset($form_state['values']['asu_comrf_new_subplan_choice'])
    && !empty($form_state['values']['asu_comrf_new_subplan_choice'])) {
    $new_subplan_choice_key = $form_state['values']['asu_comrf_new_subplan_choice'];
    $new_subplan_choice = $form_state['complete form']['asu_comrf_new_subplan_choice']['#options'][$new_subplan_choice_key];
    $new_subplan_choice_title = $form_state['complete form']['asu_comrf_new_subplan_choice']['#title'];
  }
  else {
    $new_subplan_choice_key = 'none';
  }

  $new_campus_choice_key = $form_state['values']['asu_comrf_new_campus_choice'];
  $new_campus_choice = $form_state['complete form']['asu_comrf_new_campus_choice']['#options'][$new_campus_choice_key];
  $new_campus_choice_title = $form_state['complete form']['asu_comrf_new_campus_choice']['#title'];

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

  $html_for_body = '<p><strong>' . $student_info_title . '</strong><br>' . $student_info_name . ' (' . $student_info_emplid . ')' . '</p><br><br>' .
                   '<p><strong>' . $major_to_change_title . '</strong><br>' . $major_to_change . '</p><br><br>' .
                   '<p><strong>' . $new_major_choice_title . '</strong><br>' . $new_major_choice . '</p><br><br>';

  if ($new_subplan_choice_key !== 'none') {
    $html_for_body .= '<p><strong>' . $new_subplan_choice_title . '</strong><br>' . $new_subplan_choice . '</p><br><br>';
  }

  $html_for_body .= '<p><strong>' . $new_campus_choice_title . '</strong><br>' . $new_campus_choice . '</p><br><br>' .
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
  $new_campus_choice_key = $form_state['values']['asu_comrf_new_campus_choice'];
  $new_campus_choice = $form_state['complete form']['asu_comrf_new_campus_choice']['#options'][$new_campus_choice_key];
  $com_requirements_text = $form_state['complete form']['asu_comrf_com_requirements_text']['#markup'];

  // If subplan is selected and submitted
  $new_subplan_choice = "";
  if (isset($form_state['values']['asu_comrf_new_subplan_choice'])
    && !empty($form_state['values']['asu_comrf_new_subplan_choice'])) {
    $new_subplan_choice_key = $form_state['values']['asu_comrf_new_subplan_choice'];
    $new_subplan_choice = $form_state['complete form']['asu_comrf_new_subplan_choice']['#options'][$new_subplan_choice_key];
  }
  else { // set to default value of 0
    $new_subplan_choice_key = $new_subplan_choice = 0;
  }

  // Decide whether the form should be submitted as a normal submission, or a WPC redirect submission
  switch ($submit_key) {
    case 'eAdvisor':
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
            'new_subplan_choice' => $new_subplan_choice_key,
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
            'new_campus_choice' => $new_campus_choice,
            'com_requirements_text' => $com_requirements_text,
            'requirements_check' => $submit_key,
            'double_check' => $submit_key,
            'contact_phone_number' => $submit_key,
            'major_change_source' => $submit_key,
            'major_change_source_other' => $submit_key,
            'why_change' => $submit_key,
            'why_change_other' => $submit_key,
            'timestamp' => date('U'),
            'new_subplan_choice' => $new_subplan_choice_key,
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
