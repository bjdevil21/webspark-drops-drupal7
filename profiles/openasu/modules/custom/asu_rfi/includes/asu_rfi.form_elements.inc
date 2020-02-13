<?php

function _asu_rfi_form_base_components($name, $session = FALSE, $value = '') {
  switch ($name) {
    // Category bases
    case 'about_me':
      return array(
        '#type' => 'fieldset',
        '#title' => 'About me...',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
      );
      break;
    case 'address_info':
      return array(
        '#type' => 'fieldset',
        '#title' => 'Please also send me information to the following address...',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
      );
      break;
    case 'my_interest':
      return array(
        '#type' => 'fieldset',
        '#title' => 'I\'m interested in...',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#size' => 10,
      );
      break;

    // Individual fields
    //////////////////////
    case 'req_header_text':
      return array(
        '#value' => _asu_rfi_form_header_text(),
      );
      break;
    case 'req_helper':
      return array(
        '#type' => 'item',
        '#markup' => '<p>Fields marked with <span style = "color:black;">*</span> are required.</p>',
        '#size' => 10,
      );
      break;
    case 'campus':
      return array( // Addon(s)
        '#type' => 'select',
        '#title' => t('My campus of choice is'),
        '#options' => array(),
        '#attributes' => array('class' => array('reqinfo_campus', 'chosen-disable')),
      );
      break;
    case 'student_type':
      return array(
        '#type' => 'select',
        '#title' => t('I will be a future'),
        '#options' => array(
          'First Time Freshman' => 'First Time Freshman',
          'Transfer' => 'Transfer',
          'Readmission' => 'Readmission'
        ),
        '#attributes' => array('class' => array('reqinfo_student_type', 'chosen-disable')),
        '#default_value' => isset($_SESSION['rfi_data']['student_type']) ? $_SESSION['rfi_data']['student_type'] : NULL,
      );
      break;
    case 'start_date':
      return array(
        '#type' => 'select',
        '#title' => t('My start date'),
        '#options' => $value,
        '#attributes' => array('class' => array('chosen-disable'))
      );
      break;
    case 'first_name':
      return array( // addon
        '#type' => 'textfield',
        '#title' => t('First name'),
        '#size' => 30,
        '#maxlength' => 50,
        '#required' => TRUE,
        '#prefix' => '<div id ="req_info_form">',
      );
      break;
    case 'last_name':
      return array(
        '#type' => 'textfield',
        '#title' => t('Last name'),
        '#size' => 30,
        '#maxlength' => 64,
        '#required' => TRUE,
        '#default_value' => isset($_SESSION['rfi_data']['last_name']) ? $_SESSION['rfi_data']['last_name'] : NULL,
      );
      break;
    case 'email':
      return array(
        '#type' => 'textfield',
        '#title' => t('Email'),
        '#size' => 30,
        '#maxlength' => 100,
        '#required' => TRUE,
        '#default_value' => isset($_SESSION['rfi_data']['email']) ? $_SESSION['rfi_data']['email'] : NULL,
      );
      break;
    case 'phone':
      return array(
        '#prefix' => '<div class="phone_field_rfi">',
        '#suffix' => '</div>',
        '#type' => 'textfield',
        '#title' => t('Preferred phone number'),
        '#description' => t('USA Format: xxx-xxx-xxxx'),
        '#size' => 30,
        '#maxlength' => 40,
        '#required' => TRUE,
        '#attributes' => array('class' => array('rfi_phone')),
        '#default_value' => isset($_SESSION['rfi_data']['phone']) ? $_SESSION['rfi_data']['phone'] : NULL,
      );
      break;
    case 'mobile_check':
      return array(
        '#type' => 'checkbox',
        '#title' => t('This is a USA mobile number and I would like to receive information via SMS text messaging'),
        '#title_display' => 'before',
        '#size' => 30,
        '#maxlength' => 30,
        '#required' => FALSE,
        '#attributes' => array('class' => array('rfi_mobile')),
        '#default_value' => isset($_SESSION['rfi_data']['mobile_check']) ? $_SESSION['rfi_data']['mobile_check'] : NULL,
      );
      break;
    case 'address':
      return array(
        '#type' => 'textfield',
        '#title' => t('Address'),
        '#size' => 30,
        '#maxlength' => 64,
      );
      break;
    case 'city':
      return array(
        '#type' => 'textfield',
        '#title' => t('City'),
        '#size' => 30,
        '#maxlength' => 30,
      );
    case 'state':
      return array(
        '#id' => 'rfi-state',
        '#type' => 'select',
        '#options' => $value,
        '#title' => t('State/Province'),
        '#required' => FALSE,
      );
    case 'zipcode':
      return array( // Addon
        '#type' => 'textfield',
        '#title' => t('Postal code'),
        '#size' => 30,
        '#maxlength' => 15,
        '#states' => array(
          'required' => array(
            ':input[name="country"]' => array('value' => 'United States'),
          ),
        ),
        '#default_value' => isset($_SESSION['rfi_data']['zipcode']) ? $_SESSION['rfi_data']['zipcode'] : NULL,
      );
      break;
    case 'country':
      return array(
        '#id' => 'rfi-country',
        '#type' => 'select',
        '#options' => $value,
        '#title' => t('Country'),
        '#attributes' => array('class' => array('chosen-disable')),
        '#default_value' => isset($_SESSION['rfi_data']['country']) ? $_SESSION['rfi_data']['country'] : NULL,
      );
      break;
    case 'citizenship':
      return array(
        '#type' => 'checkbox',
        '#title' => t('I am not a U.S. citizen and do not have a green card.'),
        '#title_display' => 'before',
      );
      break;
    case 'military':
      return array(
        '#type' => 'radios',
        '#title' => t('U.S. Veteran admission information and services'),
        '#options' => array(
          "None" => "None",
          "Active Duty" => "Active Duty",
          "National Guard" => "National Guard",
          "Veteran" => "Veteran",
          "Armed forces reserve" => "Armed forces reserve",
          "Spouse/Dependent" => "Spouse/Dependent"
        ),
        '#size' => 30,
      );
      break;
    case 'birthdate':
      return array(
        '#type' => 'textfield',
        '#title' => t('Date of birth'),
        '#description' => t('Format: mm/dd/yyyy'),
        '#title_display' => 'invisible',
        '#size' => 30,
        '#maxlength' => 30,
      );
    case 'questions':
      return array(
        '#type' => 'textarea',
        '#title' => t('I also need assistance with the following:'),
        '#cols' => 60,
        '#resizable' => TRUE,
        '#rows' => 5,
        '#description' => '(Limit of 255 characters)',
      );
      break;
    case 'gdpr_message':
      return array(
        '#type' => 'item',
        '#markup' => _asu_rfi_get_gdpr_markup()
      );
      break;

    // HIDDEN fields
    //////////////////
    case 'degree_type':
      return array(
        '#type' => 'hidden',
        '#size' => 30,
        '#maxlength' => 64,
        '#default_value' => (string) $value, // $form_type
      );
      break;
    case 'domain':
      return array(
        '#type' => 'hidden',
        '#size' => 30,
        '#maxlength' => 64,
        '#default_value' => (string) $_SERVER['HTTP_HOST'],
      );
      break;
    case 'enterpriseclientid':
      // Will be filled in using JS to obtain from GA, per EdPlus.
      // See WEBSPARK-1198.
      return array(
        '#type' => 'hidden',
        '#maxlength' => 255,
        '#default_value' => '',
        '#size' => 60,
      );
      break;
    case 'first_enity_id':
      return array(
        '#type' => 'hidden',
        '#default_value' => $value,
      );
      break;
    case 'ip_address':
      return array(
        '#type' => 'hidden',
        '#size' => 30,
        '#maxlength' => 64,
        '#default_value' => $_SERVER['REMOTE_ADDR'],
      );
      break;
    case 'type_of_form':
      return array(
        '#type' => 'hidden',
        '#size' => 30,
        '#maxlength' => 64,
        '#default_value' => $value,
      );
      break;
    case 'webform_url':
      return array(
        '#type' => 'hidden',
        '#size' => 30,
        '#maxlength' => 64,
        '#default_value' => $_SERVER['HTTP_HOST'],
      );
      break;
  }
  return false;
}


/**
 * @param $name
 * @param $sub_name
 * @param bool $session
 * @param null $value
 * @return mixed
 */
function _asu_rfi_form_elements_multiple_keys($name, $sub_name, $session = FALSE, $value = NULL) {
  switch ($name) {
    case 'my_interest':
      switch ($sub_name) {
        case 'campus':
          return _asu_rfi_form_base_components('campus', $session, $value);
          break;
        case 'student_type':
          return _asu_rfi_form_base_components('student_type', $session, $value);
          break;
        case 'start_date':
          return _asu_rfi_form_base_components('start_date', $session, $value);
          break;
        default:
          // none
          break;
      }
      break;
    case 'about_me':
      switch ($sub_name) {
        case 'first_name':
          return _asu_rfi_form_base_components('first_name', $session, $value);
          break;
        case 'last_name':
          return _asu_rfi_form_base_components('last_name', $session, $value);
          break;
        case 'email':
          return _asu_rfi_form_base_components('email', $session, $value);
          break;
        case 'phone':
          return _asu_rfi_form_base_components('phone', $session, $value);
          break;
        case 'mobile_check':
          return _asu_rfi_form_base_components('mobile_check', $session, $value);
          break;
        case 'country':
          return _asu_rfi_form_base_components('country', $session, $value);
          break;
        case 'zipcode':
          return _asu_rfi_form_base_components('zipcode', $session, $value);
          break;
        case 'birthdate':
          return _asu_rfi_form_base_components('birthdate', $session, $value);
          break;
        case 'citizenship':
          return _asu_rfi_form_base_components('citizenship', $session, $value);
          break;
        case 'military':
          return _asu_rfi_form_base_components('military', $session, $value);
          break;
        default:
          // none
          break;
      }
      break;
    case 'address_info':
      switch ($sub_name) {
        case 'address':
          return  _asu_rfi_form_base_components('address', $session, $value);
          break;
        case 'city':
          return _asu_rfi_form_base_components('city', $session, $value);
          break;
        case 'state':
          return _asu_rfi_form_base_components('state', $session, $value);
          break;
        case 'country':
          return _asu_rfi_form_base_components('country', $session, $value);
          break;
        case 'zipcode':
          return _asu_rfi_form_base_components('zipcode', $session, $value);
          break;
        default:
          // none
          break;
      }
      break;
    default:
      // none
      break;
  }
  flogger('asu_rfi', 'Form element @name cannot be found and rendered',
    array('@name' => $name, '@subname' => $sub_name), WATCHDOG_ERROR);
  return FALSE;
}
