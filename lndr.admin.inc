<?php
/**
 * @file
 * Code for the Lndr module
 */



/**
 * Form builder callback
 */
function lndr_admin_settings($form, &$form_state) {

  $form['lndr_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API token'),
    '#default_value' => variable_get('lndr_token', ''),
    '#description'   => t("Your Lndr account API token, you can find this information under your user profile in Lndr"),
    '#required' => TRUE,
  );

  $form['lndr_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Developer mode'),
    '#default_value' => variable_get('lndr_debug_mode', 0),
    '#description' => t('Turn this on to test Lndr API data from local source instead of executing real API call'),
  );

  return system_settings_form($form);
}

/**
 * Form validation handler
 */
function lndr_admin_settings_validate($form, &$form_state) {
  // validate the user
  $options = array(
    'method' => 'POST',
    'data' => 'token=' . $form_state['values']['lndr_token'],
  );
  $request = drupal_http_request(LNDR_API_VALIDATE_TOKEN, $options);
  if ($request->code !== '200') {
    form_set_error('lndr_token', t('You have entered an invalid API token, please copy and paste the API token from your profile in Lndr'));
  } else {
    // @todo: in the future we can do additional check such as see if that user is inactive, etc.
    variable_set('lndr_api_info', json_decode($request->data, true));
  }
}