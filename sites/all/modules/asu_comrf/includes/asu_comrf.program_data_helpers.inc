<?php

/**
 * Determines if at least one week has passed since the last check time.
 *
 * Returns TRUE if 24 hours has passed since last cron run AND
 * 1) It is before 7AM AZ standard time, OR
 * 2) A week has passed without running.
 * Else: Returns FALSE
 * @return bool
 */
function _asu_comrf_cron_period_check() {
  // Number of seconds in a day
  define('DAY_IN_SECONDS', 86400);
  define('WEEK_IN_SECONDS', 604800);

  // Gets difference between time stamp of last cron check and current time stamp.
  // Run the update if last check time can't be found
  $time_lapse = time() - variable_get('asu_comrf_check_time', 1);

  // Returns TRUE if last cron was run more than a week ago and it is before 7AM local AZ time.
  $tz_obj = new DateTimeZone('America/Phoenix');
  $today = new DateTime("now", $tz_obj);
  $current_hour = (int) $today->format('H');
  return ($current_hour < 7 || $time_lapse > WEEK_IN_SECONDS) ? TRUE : FALSE;
}

/**
 * Determines the next term code for Degree Search.
 *
 * @return string
 */
function _asu_comrf_get_next_term_code() {
  // The first digit of '2' indicates that the year is at least 2000 ('0' was 19XX)
  // The second and third digits give you the rest of the year (so '213X' is 2013)
  // The fourth digit is semester.
  // '1' = spring
  // '7' = fall
  // '9' = winter
  // '4' = summer
  // '5' = summer session 2 (no longer used, but it might show up if we're pulling old enough data
  //     -- we should probably just merge 5 with 4)

  // Set date-based variables
  $current_millennium = '2';
  $current_year = date('y');
  $current_month = date('n');

  // Initialize term identifier
  $term_identifier = '';

  // Determine term identifier
  switch($current_month) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      $term_identifier = '7';
      break;
    case '8':
    case '9':
    case '10':
    case '11':
    case '12':
      $term_identifier = '1';
      $current_year += 1;
      break;
    default:
      break;
  }

  // Build term code
  $next_term = $current_millennium . $current_year . $term_identifier;

  return $next_term;
}

/**
 * Get individual student information from PeopleSoft data to display on form above fields.
 *
 * @return array
 */
function asu_comrf_get_student_info($asurite_id) {

  // Set Mulesoft client ID and secret
  $client_id = variable_get('asu_comrf_mulesoft_api_client_id');
  $client_secret = variable_get('asu_comrf_mulesoft_api_client_secret');
  $api_env = variable_get('asu_comrf_change_major_api_student_env');
  $api_url = variable_get('asu_comrf_change_major_api_student_url_' . $api_env);
  $student_info_base_url = 'https://' . $client_id . ':' . $client_secret . '@' . $api_url;
  $student_info_url = $student_info_base_url . '?id=' . $asurite_id;

  $mulesoft_auth = variable_get('asu_comrf_change_major_api_email_env', '');

  $request_options = array(
    'headers' => array(
      'Authorization' => $mulesoft_auth,
    ),
    'method' => 'GET',
  );

  $result = drupal_http_request($student_info_url, $request_options);

  if ($result->code <> 200) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call URL (sans app_ID:key) for ASURITE ID '
      . $asurite_id . ': ' . $api_url  . ' - Mulesoft call return code ' . $result->code, array(), WATCHDOG_DEBUG);
  }

  // See https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html
  // for error codes list.
  if (($result->code > 0) && ($result->code < 125)) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call for ASURITE ID ' . $asurite_id . ' failed with system error ' . $result->code
      . '. Check Mulesoft API to see if it is working.', array(), WATCHDOG_ERROR);
    drupal_goto("request/error");
    drupal_exit();
  } elseif (isset($result->error)) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call for ASURITE ID ' . $asurite_id . ' not working. Returns error ' . check_plain($result->error),
      array(), WATCHDOG_ERROR);
    drupal_goto("request/notice");
    drupal_exit();
  }

  $student_info_raw = json_decode($result->data)->studentMajor;
  $student_info_raw = $student_info_raw[0];
  $student_info = asu_comrf_build_student_profile($student_info_raw);

  return $student_info;
}

/**
 * Builds the student profile for the student information block via asu_comrf_get_student_info().
 *
 * @param $student_info_raw
 * @return array
 */
function asu_comrf_build_student_profile($student_info_raw) {

  // Initialize array for degree programs
  $degree_programs = array();

  // Get student's name and careers to parse through
  $student_info['name'] = $student_info_raw->firstName . ' ' . $student_info_raw->lastName;
  $student_info['emplid'] = $student_info_raw->emplid;
  $careers = $student_info_raw->careers;

  if (empty($careers)) {
    drupal_goto("request/notice");
    drupal_exit();
  }

  // Initialize variable to count number of MAJ or PRE plan types for a given student
  $plan_type_program_count = 0;

  // Only display undergraduate majors
  foreach ($careers as $career) {
    if ($career->career == 'UGRD') {
      foreach ($career->plans as $plan) {
        if ($plan->planType == 'MAJ' || $plan->planType == 'PRE') {
          $plan_name = $plan->transcriptDescription;
          $degree_type = $plan->degree;
          $current_program = array(
            'plan_name' => $plan_name,
            'degree_type' => $degree_type,
          );
          array_push($degree_programs, $current_program);
          $plan_type_program_count++;
        }
      }
    }
  }

  // Send user to notice page if they don't have at least one MAJ or PRE plan type
  if ($plan_type_program_count == 0) {
    drupal_goto("request/notice");
    drupal_exit();
  }

  // Add MAJ plan type degree programs to student profile
  $student_info['degrees'] = $degree_programs;

  return $student_info;
}

/**
 * Returns the current degree program list from the database.
 *
 * @return mixed
 */
function asu_comrf_get_current_program_list() {
  // Retrieve degree program list from database and unserialize the data for processing
  $list_of_degrees_query = db_query("SELECT degree_programs FROM {asu_comrf_degree_list} ORDER BY lid DESC LIMIT 1");
  $list_of_degrees_results_array_s = $list_of_degrees_query->fetchAll();
  $list_of_degrees_results_s = $list_of_degrees_results_array_s[0]->degree_programs;
  $list_of_degrees_results = unserialize($list_of_degrees_results_s);

  // Check for malformed data returned from Degree Search (e.g. 'Degree' field is empty,
  // therefore it defaults to an object, which causes errors down the line)
  if ($list_of_degrees_results) {
    for ($i = 0; $i < count($list_of_degrees_results); $i++) {
      if (is_object($list_of_degrees_results[$i]['TrnscrDescr'])) {
        $list_of_degrees_results[$i]['TrnscrDescr'] = '';
      }
      if (is_object($list_of_degrees_results[$i]['Degree'])) {
        $list_of_degrees_results[$i]['Degree'] = '';
      }
      if (is_object($list_of_degrees_results[$i]['AcadPlan'])) {
        $list_of_degrees_results[$i]['AcadPlan'] = '';
      }
    }
  }

  return $list_of_degrees_results;
}

/**
 * Translates Peoplesoft-esque campus codes into a human-readable format.
 *
 * @param $new_campus_choice_raw
 * @return array
 */
function _asu_comrf_translate_campus_code_options($new_campus_choice_raw) {
  // Initialize campus choice array to return
  $new_campus_choice = array();
  // Translate campus codes to human-readable names
  foreach ($new_campus_choice_raw as $key => $campus_raw) {
    switch ($campus_raw) {
      case 'ONLNE':
        $new_campus_choice[$key] = 'Online';
        break;
      case 'DOWNTOWN':
      case 'DTPHX':
        $new_campus_choice[$key] = 'Downtown';
        break;
      case 'POLY':
        $new_campus_choice[$key] = 'Polytechnic';
        break;
      case 'TMPE':
      case 'TEMPE':
        $new_campus_choice[$key] = 'Tempe';
        break;
      case 'WEST':
        $new_campus_choice[$key] = 'West';
        break;
      case 'CALHC':
        $new_campus_choice[$key] = 'Lake Havasu City';
        break;
      case 'CAC':
        $new_campus_choice[$key] = 'ASU@Pinal';
        break;
      case 'EAC':
        $new_campus_choice[$key] = 'ASU@TheGilaValley';
        break;
      case 'TUCSN':
        $new_campus_choice[$key] = 'ASU@Tucson';
        break;
      case 'AWC':
        $new_campus_choice[$key] = 'ASU@Yuma';
        break;
      case 'TBIRD':
        $new_campus_choice[$key] = 'Thunderbird';
        break;
      case 'YAVAP':
        $new_campus_choice[$key] = 'ASU@Yavapai';
        break;
      case 'PIMA':
        $new_campus_choice[$key] = 'ASU@Pima';
        break;
      case 'COCHS':
        $new_campus_choice[$key] = 'ASU@Cochise';
        break;
      case 'MXCTY':
        $new_campus_choice[$key] = 'ASU@MexicoCity';
        break;
      case 'WASHD':
        $new_campus_choice[$key] = 'ASU@Washington, D.C.';
        break;
      default:
        $new_campus_choice[$key] = $campus_raw;
        break;
    }
  }
  return $new_campus_choice;
}

/**
 * Helper function - Get all subplans for an academic plan,
 * and alter the form and form_state variables
 *
 * @param $list_of_subplans (mixed) - Returned list of subplans for a given acadPlan code.
 * @param $form (array) - Contains CofM $form settings for Drupal API
 */
function _asu_comrf_render_subplans_list($list_of_subplans, &$form) {
  $new_subplan_options = array();

  // Does subplan actually exist (in a non-empty object for one option
  // or array for two or more options)?
  if ((is_object($list_of_subplans) && !empty($list_of_subplans->name)) ||
    (is_array($list_of_subplans) && isset($list_of_subplans[0]))) {

    if (is_object($list_of_subplans)) { // Only one subplan object
      $new_subplan_options[$list_of_subplans->name] = $list_of_subplans->value;
    }
    else { // two or more subplans objects are wrapped by an array
      foreach($list_of_subplans as $sub => $plan) {
        $new_subplan_options[$plan->name] = $plan->value;
      }
    }
    $form['asu_comrf_new_subplan_choice']['#options'] = $new_subplan_options;
    $form['asu_comrf_new_subplan_choice']['#required'] = TRUE;
    $subplan_display = 'block';
  }
  else { // No subplans are available, so don't make any changes
    $form['asu_comrf_new_subplan_choice']['#options'] = array(0 => 'No subplans available');
    $form['asu_comrf_new_subplan_choice']['#required'] = FALSE;
    $subplan_display = 'none';
  }
  // Subplan CSS toggle
  $form['asu_comrf_new_subplan_css']['#markup'] = '<style type="text/css">#asu-comrf-new-subplan-choice-ajax-wrapper { display: ' . $subplan_display . '; } </style>';
}
