<?php

/**
 * Implements hook_menu_alter
 * @param $items
 */
function lndr_menu_alter(&$items) {
  // We are rerouting how node is displayed.
  $items['node/%node']['page callback'] = 'lndr_node_reroute';
}

/**
 * Implements hook_node_load
 * @param $nodes
 * @param $types
 */
function lndr_node_load($nodes, $types) {
  // Add metadata about lndr for the mapped content
  $bundle = variable_get('lndr_config_ct', 'page');
  foreach($nodes as $nid => $node) {
    if ($node->type === $bundle) {
      $lndr_data = variable_get('lndr_node_' . $nid, array());
      if (!empty($lndr_data)) {
        $nodes[$nid]->lndr_data = $lndr_data;
      }
    }
  }
}

/**
 * We are using this to reroute the external page to our own custom callback.
 * @param $node
 */
function lndr_node_reroute($node) {
  // Get configuration
  $lndr_ct = variable_get('lndr_config_ct', 'page');
  if (isset($lndr_ct) && $node->type === $lndr_ct) {
    lndr_view_node($node);
  } else {
    // default way of viewing a node
    return node_page_view($node);
  }
}

/**
 * Creates a new node for lndr landing page
 * This happens when user creates a page (not published) to this Drupal domain
 * @param $landing_page
 */
function _lndr_create_content($landing_page) {
  // @todo: build in default content type per this module
  // @todo: sync the $landing_page param with what we get from web service
  $bundle = variable_get('lndr_config_ct', 'page');

  $node = new stdClass();
  $node->type = $bundle;
  node_object_prepare($node);
  $node->language = LANGUAGE_NONE;
  $node->status = 0;
  $node->title = $landing_page['title'];
  $node->path = $landing_page['path'];
  $node->data = array(
    'lndr' => array(
      'page_id' => $landing_page['page_id'],
      'internal_url' => $landing_page['internal_url'],
      'publishing_url' => $landing_page['external_url'],
    ),
  );
  node_save($node);
  // @todo: somewhere outside of this function we should validate if existing path exist.
  if (isset($node->nid)) {
    // Save the node path
    $path = array(
      'source' => 'node/' . $node->nid,
      'alias' => $node->path,
    );
    path_save($path);
    // Save additional data of that node, maybe later use a full db table
    variable_set('lndr_node_' . $node->nid, $landing_page);
  }
}

/**
 * Import and view an individual node as Lndr landing page
 * @param $node
 */
function lndr_view_node($node) {
  // Check if that page has lndr meta-data
  if(property_exists($node, 'lndr_data')) {
    if (!empty($node->lndr_data)) {
      _lndr_import_page($node->lndr_data['internal_url']);
    }
  }
}