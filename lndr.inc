<?php
/**
 * @file
 * ${NAME} class implementation.
 */

/**
 * Gets an directory by origin url.
 *
 * @param $project
 *    Lndr project object.
 *
 * @return array
 *    Reverse parsed url.
 */
function lndr_get_origin_directory($project) {
  $url_parts = explode('/', trim($project["origin_url"], '\/'));
  return array_reverse($url_parts);
}

/**
 * Gets an public url by project.
 *
 * @param $project
 *    Lndr project object.
 *
 * @return string
 *    Public drupal folder string.
 */
function lndr_get_public_by_project($project) {
  return 'public://lndr/' . $project['id'];
}

/**
 * Returns an array of all project from Lndr service.
 *
 * @return array
 */
function lndr_get_projects() {
  $result = &drupal_static(__FUNCTION__);
  if (!isset($result)) {
    $result = lndr_client_get_projects();
  }
  return $result;
}

/**
 * Return project by property from projects array.
 *
 * @param $prop
 *    Property name.
 * @param $value
 *    The searched value.
 *
 * @return array|object|bool
 */
function lndr_find_project_by_prop($prop, $value) {
  $data = lndr_get_projects();
  $search_res = array_search($value, array_column(((array) $data)['projects'], $prop));
  return (FALSE !== $search_res) ? ((array) $data)['projects'][$search_res] : FALSE;
}

/**
 * Return project by url from projects array.
 *
 * @param $path
 *    The searched path.
 *
 * @return array|object|bool
 */
function lndr_find_project_by_url($path) {
  return lndr_find_project_by_prop('publish_url', url($path, array('absolute' => TRUE)));
}

/**
 * Transfers a file to the client using HTTP.
 *
 * @param $node
 *    Lndr node object.
 *
 * @param $target
 *    File name.
 */
function lndr_file_transfer($node, $target) {
  $lang = $node->language;
  $lndr_project_id = $node->lndr_project_id[$lang][0]['value'];
  $project = lndr_find_project_by_prop('id', $lndr_project_id);

  $directory = lndr_get_public_by_project($project);
  $image_uri = $directory . '/' . $target;
  if (!file_exists($image_uri)) {
    lndr_save_unmanaged_files($target, $directory, $project['origin_url']);
  }
  $headers = file_download_headers($image_uri);
  // @TODO If headers are empty return MENU_ACCESS_DENIED.
  if (count($headers)) {
    foreach ($headers as $name => $value) {
      drupal_add_http_header($name, $value);
    }
  }
  file_transfer($image_uri, array());
}

/**
 * Create an unpublished node to hold the URL path
 * string $path.
 *
 * @param string $path
 * @param string $lndr_id
 *
 * @return \stdClass
 * @throws \Exception
 */
function lndr_reserve_node($path, $lndr_id) {
  $node = new stdClass();
  $node->title = $path;
  $node->type = LNDR_CT_TYPE;
  node_object_prepare($node);

  // @TODO These three properties will be overridden, but it cen be defined by default content type configuration.
  // @see node_object_prepare().
  // @file lndr.install It is not implemented yet.
  $node->uid = 0;
  $node->status = 1;
  $node->promote = 0;

  $node->language = LANGUAGE_NONE;
  $node->lndr_project_id[$node->language] = array(
    array('value' => $lndr_id),
  );

  $node->path['alias'] = $path;
  node_save($node);

  return $node;
}

/**
 * Checknig a path exists.
 *
 * @param string $path
 *
 * @return string
 *    Existing path.
 */
function lndr_check_path_exiting($path) {
  return path_load(array('alias' => $path));
}

/**
 * Updating an existing alias.
 * @param int $nid
 * @param string $path
 *
 * @throws \Exception
 */
function lndr_update_node($nid, $path){
  $node_to_be_updated = node_load($nid);
  $node_to_be_updated->path['alias'] = $path;
  node_save($node_to_be_updated);
}
