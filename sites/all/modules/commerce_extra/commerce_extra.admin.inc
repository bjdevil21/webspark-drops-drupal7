<?php
/**
 * @file
 * Commerce Extra settings for admins.
 */

/**
 * Form callback for module settings.
 */
function commerce_extra_settings($form, &$form_state) {

  // We need this for finding out which submodules are enabled/disabled
  $module_list = module_list();

  // List features
  foreach (_commerce_extra_features() as $feature => $info) {
    $form[$feature] = array(
      '#type' => 'fieldset',
      '#title' => $info['title'],
      '#description' => $info['description'],
    );
    $form[$feature][$feature . '_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#default_value' => isset($module_list[$feature]),
    );
    _commerce_extra_attach_feature_configuration($form);
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Form validation callback for module settings.
 */
function commerce_extra_settings_validate(&$form, &$form_state) {

  // We need this for finding out which submodules are enabled/disabled
  $module_list = module_list();

  // Loop thru
  foreach (_commerce_extra_features() as $feature => $info) {
    // If module is enabled
    // TODO: Should probably call for features which are going to be enabled
    // but aren't currently enabled yet.
    if (isset($module_list[$feature])) {
      // Invoke validation callback
      $function = $feature . '_commerce_extra_configure_validate';
      if (function_exists($function)) {
        $function($form, $form_state);
      }
    }
  }

}

/**
 * Form submission callback for module settings.
 */
function commerce_extra_settings_submit($form, &$form_state) {

  // We need this for finding out which submodules are enabled/disabled
  $module_list = module_list();
  $enable = array();
  $disable = array();

  // Loop thru
  foreach (_commerce_extra_features() as $feature => $info) {
    // If module was enabled
    // TODO: Should probably call for features which are going to be enabled
    // but aren't currently enabled yet.
    if (isset($module_list[$feature])) {
      // Invoke submission callback
      $function = $feature . '_commerce_extra_configure_submit';
      if (function_exists($function)) {
        $function($form, $form_state);
      }
    }
    // If new settings differs from current state
    if ($form_state['values'][$feature . '_enable'] != isset($module_list[$feature])) {
      
      if ($form_state['values'][$feature . '_enable']) {
        // We want enable the submodule
        $enable[] = $feature;
      }
      else {
        // We want disable the submodule
        $disable[] = $feature;
      }
    }
  }

  // Enable/disable possible modules
  if (count($enable) > 0) module_enable($enable);
  if (count($disable) > 0) module_disable($disable);

}

/**
 * Helper function for getting list of features.
 * @return
 *   Returns associative array containing features and their details.
 */
function _commerce_extra_features() {
  return array(
    'commerce_extra_address_populate' => array(
      'title' => t('Address Populate'),
      'description' => t('Uses user account address field to pre-populate address information in checkout. After enabling, <a href="@url">edit your settings</a> of "Billing information" pane or other customer profile pane.', array('@url' => url('admin/commerce/config/checkout/form/pane/customer_profile_billing'))),
    ),
    'commerce_extra_login_page' => array(
      'title' => t('Login Page'),
      'description' => t('Creates extra step for checkout process where user can alternatively login.'),
    ),
    'commerce_extra_quantity' => array(
      'title' => t('Quantity'),
      'description' => t('Wraps quantity fields with - and + links to decrease or increase quantity.'),
    ),
  );
}

/**
 * Helper function for attaching configuration form to Commerce Extra's main
 * configuration form.
 *
 * TODO: Configuration probably should be always attached so that #states
 * would work for currently disabled submodules too.
 */
function _commerce_extra_attach_feature_configuration(&$form) {
  foreach (_commerce_extra_features() as $feature_name => $feature) {
    $function = $feature_name . '_commerce_extra_configure';
    if (function_exists($function)) {
      $attach_elements = $function();
      if (is_array($attach_elements)) {
        foreach ($attach_elements as $element_key => $element) {
          $form[$feature_name][$element_key] = $element;
          $form[$feature_name][$element_key]['#states'] = array(
            'visible' => array(
              ':input[name="' . $feature_name . '_enable"]' => array('checked' => TRUE)
            ),
          );
        }
      }
    }
  }
}
