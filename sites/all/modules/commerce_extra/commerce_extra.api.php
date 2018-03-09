<?php
/**
 * @file
 * API documentation for Commerce Extra feature implementation.
 */

/**
 * CALLBACK_commerce_extra_configure()
 */
function CALLBACK_commerce_extra_configure() {
  // Example from commerce_extra_quantity feature
  $form = array();
  $form['commerce_extra_quantity_cardinality'] = array(
    '#type' => 'textfield',
    '#title' => t('Quantity cardinality'),
    '#description' => t('You can change quantity cardinality for quantity widget.'),
    '#size' => 3,
    '#default_value' => variable_get('commerce_extra_quantity_cardinality', '1'),
  );
  return $form;
}

/**
 * CALLBACK_commerce_extra_configure_validate()
 */
function CALLBACK_commerce_extra_configure_validate(&$form, &$form_state) {
  // Example from commerce_extra_quantity feature
  if (!is_numeric($form_state['values']['commerce_extra_quantity_cardinality'])) {
    form_set_error('commerce_extra_quantity_cardinality', t('Field @field_name must be numeric value.', array('@field_name' => t('Quantity cardinality'))));
  }
}

/**
 * CALLBACK_commerce_extra_configure_submit()
 */
function CALLBACK_commerce_extra_configure_submit(&$form, &$form_state) {
  // Example from commerce_extra_quantity feature
  variable_set('commerce_extra_quantity_cardinality', $form_state['values']['commerce_extra_quantity_cardinality']);
}
