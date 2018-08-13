<?php

/**
 * @file
 * Code for the Lndr administration pages.
 */

/**
 * Form builder callback.
 */
function lndr_admin_settings($form, &$form_state) {
  $form['lndr_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API token'),
    '#default_value' => variable_get('lndr_token', ''),
    '#description' => t("Your Lndr account API token, you can find this information under your user profile in Lndr"),
    '#required' => TRUE,
  );

  if (module_exists('lndr_debug')) {
    $form['lndr_debug_mode'] = array(
      '#type' => 'checkbox',
      '#title' => t('Developer mode'),
      '#default_value' => variable_get('lndr_debug_mode', 0),
      '#description' => t('Turn this on to test Lndr API data from local source instead of executing real API call'),
    );
  }
  $desc = <<<EOF
Delivery mode - If some resources haven't prepared via cron, page will be able to render and new sources will be downloaded on client side.<br/>
Performance mode - Any resources will be delivered via cron only.
EOF;
  $form['lndr_delivery_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Delivery mode'),
    '#options' => array(
      'delivery' => t('Delivery'),
      'performance' => t('Performance'),
    ),
    '#default_value' => variable_get('lndr_delivery_mode', 'performance'),
    '#description' => t($desc),
  );

  return system_settings_form($form);
}

/**
 * Form validation handler.
 */
function lndr_admin_settings_validate($form, &$form_state) {
  // Validate the user.
  $options = array(
    'method' => 'POST',
    'data' => 'token=' . $form_state['values']['lndr_token'],
  );
  $request = drupal_http_request(LNDR_API_VALIDATE_TOKEN, $options);
  if ($request->code !== '200') {
    form_set_error('lndr_token', t('You have entered an invalid API token, please copy and paste the API token from your profile in Lndr'));
  }
  else {
    // @todo: in the future we can do additional check such as see if that user is inactive, etc.
    // @TODO We need use somewhere this variable.
    variable_set('lndr_api_info', json_decode($request->data, TRUE));
  }
}
