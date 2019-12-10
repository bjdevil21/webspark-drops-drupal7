<?php

/**
 * POSTs form data to Salesforce for UNDERGRADUATE submissions.
 * Done one submission at a time.
 * @param array $data
 * @param string $type - UGrad or Graduate (constants)
 */
function asu_rfi_undergrad_post($data, $type) {
  global $base_url;
  $form_mode = variable_get('asu_rfi_form_mode');

  // Pull up RFI submission node
  $degreeMappings = _asu_rfi_degree_field_mappings();
  $nid = $data['submission_nodeid'];
  $node = node_load($nid);
  $nodeArr = array(
    $node->field_rfi_program['und'][0]['value'] => $node->field_rfi_program_name['und'][0]['value']
  );
  $availableCampuses = _asu_rfi_campus_location_options($nodeArr);

  // Pull up RFI submission node
  $node_wrapper = entity_metadata_wrapper('node', $node);

  // Get information for logging
  $nid_info = (isset($node_wrapper->nid)) ? ' for NID ' . $node_wrapper->nid->value() : '';

  if (isset($node_wrapper->field_rfi_submission_status)) {
    if ($node_wrapper->field_rfi_submission_status->value() == "Incomplete") {
      $node_wrapper->field_rfi_submission_status->set('Pending');
      $node_wrapper->save();
    } else {
      if ($node_wrapper->field_rfi_submission_status->value() == "Pending") {
        $node_wrapper->field_rfi_submission_status->set('action_required');
        $node_wrapper->save();
      }
    }
  }

  // Start building data for middleware submission
  $prog_code = $data['program_code'];
  $campus = _asu_rfi_get_correct_campus_id($data['campus']);
  $phone = preg_replace('[\D]', '', $data['phone']);
  $mobile = (isset($data['mobile_check']))
    ? str_replace('-', '', $data['mobile_check'])
    : ''; // mobile phone
  $dob = (isset($data['birthdate'])) ? $data['birthdate'] : ''; // date of birth
  $military = (isset($data['military'])) ? $data['military'] : '';  // Military
  $comment = (isset($data['questions'])) ? $data['questions'] : ''; // Comment

  $location = (isset($data['location'])) ? $data['location'] : ''; // Location
  // Online campus check
  if ($location === "no_preference" || $type === ASU_RFI_PROSPECT_LEVEL_GRAD) {
    if (!isset($campus) && sizeof($availableCampuses) == 1
      && (ucfirst($availableCampuses[0]) == "Online")) {
      $location = "Online";
    }
  }

  $address = (isset($data['address'])) ? $data['address'] : ''; // Address
  $city = (isset($data['city'])) ? $data['city'] : ''; // City
  $state = (isset($data['state'])) ? $data['state'] : ''; // State
  $country = (isset($data['country'])) ? $data['country'] : ''; // country

  // GA enterpriseclientid for use in EdPlus Leads submissions.
  $enterpriseclientid = (isset($data['enterpriseclientid'])) ? $data['enterpriseclientid'] : '';

  // Get authentication key
  $mw_auth_key = variable_get('asu_rfi_form_auth_key');

  // Get server diagnostic data for submissions
  $server = _asu_rfi_detect_source($data, $type);

  // Submission customization - UNDERGRAD vs GRAD
  if ($type === ASU_RFI_PROSPECT_LEVEL_UNDERGRAD) {
    $hashkey = sha1($mw_auth_key . '+' . $nid);
    // Default submission URL (non-Online)
    $url = "https://webforms.asu.edu/asu_rfi_dedupe/submit_rfi/$hashkey/$nid";

    // Get college code from database based on program code
    if ($prog_code == "" || empty($prog_code)) {
      $prog_code = "Exploratory";
      $college_code = null;
    } else {
      $college_code = _asu_rfi_get_college_code($degreeMappings, $prog_code);
    }

    // Compile DEFAULT Undergrad RFI submission data array
    $submission_data = array(
      'source_id' => $server['source'],
      'base_url' => $server['domain'],
      'prod_test_flag' => $form_mode,
      'first_name' => $data['first_name'],
      'last_name' => $data['last_name'],
      'prospect_level' => ASU_RFI_PROSPECT_LEVEL_UNDERGRAD,
      'student_type' => $data['student_type'],
      'program' => $prog_code,
      'starting_semester' => $data['start_date'],
      'email' => $data['email'],
      'zip' => $data['zipcode'],
      'phone' => $phone,
      'birth_date' => $dob,
      'country' => $country,
      'sms' => $mobile,
      'comments' => $comment,
      'street' => $address,
      'city' => $city,
      'state' => $state,
      'intnl_student' => $data['citizenship'],
      'veteran_status' => $military,
      'campus' => $campus,
      'location' => $location,
    );

    // Build default submission for email
    $body = json_encode($submission_data);
  }

  if ($type === ASU_RFI_PROSPECT_LEVEL_GRAD) {
    // Semester term name
    $term = (isset($data['semester_name'])) ? $data['semester_name'] : '';
    // If NEITHER the USA NOR Canada, drop the address fields' data.
    if ($country !== "United States" && $country !== "Canada") {
      $address = '';
      $state = '';
      $city = '';
    }

    // Select proper ENV for Salesforce GRAD POSTs.
    // Is it a dev or qa site, or in "test mode"?
    $environ = (array) _asu_rfi_detect_environment();
    if (($environ['type'] === "dev") || ($environ['type'] === "qa")) {
      $url = 'https://requestinfo-qa.asu.edu/prospect_form_post';
    } else {
      if ($form_mode == 'test') {
        $url = 'https://requestinfo-qa.asu.edu/prospect_form_post';
      }
      else {
        $url = 'https://requestinfo.asu.edu/prospect_form_post';
      }
    }

    // Build DEFAULT Graduate data array for Salesforce (the camelCase field
    // keys don't match the usual key formatting).
    $submission_data = array(
      'source' => $server['source'],
      'firstName' => $data['first_name'],
      'lastName' => $data['last_name'],
      'emailAddress' => $data['email'],
      'phoneNumber' => $phone,
      'projectedEnrollment' => $term,
      'addressLine1' => $address,
      'city' => $city,
      'state' => $state,
      'countryOfResidence' => $country,
      'birthdate' => $dob,
      'militaryStatus' => $military,
      'questions' => $comment,
      'poiCode' => $prog_code,
      'zip' => $data['zipcode'],
      'iStudent' => $data['citizenship'],
      'smsNumber' => $mobile,
      'campusSelection' => $campus,
    );
  }

  // Alter Online campus submission data
  if (strtolower($location) === "online") {
    if ($type === ASU_RFI_PROSPECT_LEVEL_GRAD) {
      // Match different "Online" campus spelling
      $submission_data['campusSelection'] = "ONLNE";
    }
    $asuOnlineList = _asu_rfi_parse_programs();
    $managedOnline = _asu_rfi_check_managed_online($prog_code, $asuOnlineList);

    if ($managedOnline == true) {
      if ($type === ASU_RFI_PROSPECT_LEVEL_GRAD) {
        $college_code = _asu_rfi_get_college_code($degreeMappings, $prog_code);
        // Make sure all submissions to webforms.asu.edu are attempted.
        $environ['endpoint_access'] = true;
      }
      $submission_data = _asu_rfi_online_fields_alter($type, $submission_data, $server, $college_code, $enterpriseclientid, $nid, $form_mode);
      $body = json_encode($submission_data);
      $fieldHash = sha1($mw_auth_key . '+' .  $body);
      $url = "https://webforms.asu.edu/asu_edplus_middleware/submit/$fieldHash";
    }
  } else { // NOT online campus
    foreach ($submission_data as $key_value => $result_array) {
      if (($result_array == "None")) {
        unset($submission_data[$key_value]);
      }
    }
  }

  //////////////////////////////////

  ///////////////////////////////////
  // Post data to SalesForce middleware via cURL
  // NOTE: Unlike the graduate level submissions, the Salesforce endpoint for
  // testing submissions is publicly available for undergraduate submissions.
  ///////////////////////////////////
  try {
    $ch = curl_init();
    $options = array(
      CURLOPT_URL => $url,
      CURLOPT_POST => 1,
      CURLOPT_POSTFIELDS => $body,
      CURLOPT_RETURNTRANSFER => 1,
      CURLOPT_HTTPHEADER => array('Content-Type: text/json'),
      CURLOPT_SSL_VERIFYPEER => FALSE,
    );
    curl_setopt_array($ch, $options);
    $response = curl_exec($ch);
    $curl_info = curl_getinfo($ch);

    // Successful CURL submission to middleware
    if (($curl_info['http_code'] == 200)) {
      $curl_message = 'SF SUCCESS - Posted undergrad form submission'
        . $nid_info . ' to ' . $url;
      if (isset($node_wrapper->field_rfi_error_message)) {
        $node_wrapper->field_rfi_error_message->set('');
      }
      if (isset($node_wrapper->field_rfi_submission_status)) {
        $node_wrapper->field_rfi_submission_status->set('SF_Complete');
      }
      $node_wrapper->save();
      watchdog('asu_rfi', $curl_message, array(), WATCHDOG_INFO);
      variable_set('asu_rfi_last_success_SF_posting', time());
    }
    // Failed CURL submission to SF middleware
    // Save the error message in the submission node and send message
    // onto the confirmation page screen. And set the submission status to
    // Incomplete.
    else {
      $curl_message = (isset($node_wrapper->field_rfi_error_message))
        ? 'SF FAILURE - cURL POST '
        . $nid_info . ' to ' . $url . ' -- HTTP CODE: ' . $curl_info['http_code']
        . ' - ' . $response
        : '';
      // if ($response == "Access Denied - hashed value does not match any registered site") {
      if (($curl_info['http_code'] >= 400)) {

        $node_wrapper->field_rfi_error_message->set($curl_message);
        //$node_wrapper->field_rfi_submission_status->set('Incomplete');  // Why wasn't this here before???
        $node_wrapper->save();

        // Get admin email address from admin settings page
        $to_email = variable_get('asu_rfi_error_email', NULL);
        // Get middleware app key. If it's empty, don't send error email to admins.
        if (!empty($mw_auth_key)) {
          if (!empty($to_email)) {
            if (!isset($data['cron_run']) || ($data['cron_run'] != "Cron_running")) {
              _asu_rfi_failed_post_email($to_email, $nid);
            }
          }
        }
      }
    }
    curl_close($ch);

    // Output result of cURL action for admins
    if (drupal_valid_path('admin')) {
      drupal_set_message($curl_message);
    }

    if (FALSE === $response) {
      throw new Exception(curl_error($ch), curl_errno($ch));
    }
  }
  catch (Exception $e) {
    trigger_error(sprintf(
      'Curl failed with error #%d: %s',
      $e->getCode(), $e->getMessage()),
      E_USER_ERROR);
  }
}
