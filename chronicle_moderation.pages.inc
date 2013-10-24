<?php

/**
 * @file
 * Provides display output for moderation pages.
 */

/**
 * Menu callback: display the moderation dashboard page.
 */
function chronicle_moderation_page() {
  $build = array();

  static $states;
  if (!isset($states)) {
    $states = array();
    foreach (workflow_get_workflow_states() as $state) {
      $states[$state->sid] = $state->state;
    }
  }

  $sql = 'SELECT n.nid AS nid,
      n.language AS node_language,
      wn.uid AS transition_uid,
      wn.stamp AS transition_stamp,
      wnh.old_sid AS old_state,
      wnh.comment AS workflow_comment
    FROM
      workflow_node wn
      LEFT JOIN workflow_node_history wnh ON (
        wn.nid = wnh.nid
        AND wn.stamp = wnh.stamp
      )
      LEFT JOIN node n ON wn.nid = n.nid
      LEFT JOIN node_revision nr ON n.vid = nr.vid
    WHERE wn.sid = :current_state
    ORDER BY transition_stamp ASC
  ';

  if (!$transitions = variable_get('chronicle_moderation_transitions', FALSE)) {
    return array(
      '#markup' => t('There are no workflow transitions configured.')
    );
  }

  foreach ($transitions as $key => $title) {
    $result = db_query($sql, array(':current_state' => $key));

    $rows = array();
    $header = array();
    foreach ($result as $item) {
      $node = node_load($item->nid);
      if (!node_access('update', $node)) continue;

      $item->old_state = $states[$item->old_state];
      $revision_account = user_load($node->revision_uid);
      $transition_account = user_load($item->transition_uid);

      $rows[$node->nid] = array(
        array(
          'data' => l($node->title, 'node/' . $node->nid),
        ),
        array(
          'data' => format_date($node->revision_timestamp, 'short'),
        ),
        array(
          'data' => l($revision_account->name, 'user/' . $revision_account->uid),
        ),
        array(
          'data' => $item->old_state,
        ),
        array(
          'data' => format_date($item->transition_stamp, 'short'),
        ),
        array(
          'data' => l($transition_account->name, 'user/' . $transition_account->uid),
        ),
        array(
          'data' => $item->workflow_comment,
        ),
      );
    }

    $header = array(
      t('Title'),
      t('Latest revision'),
      t('Revision author'),
      t('Previous state'),
      t('State changed date'),
      t('State changed by'),
      t('Comment'),
    );

    if (!empty($rows)) {
      $build[$key] = array(
        '#prefix' => '<h2>' . $title . '</h2>',
        '#suffix' => '<br />',
        'list' => array(
          '#theme' => 'table',
          '#header' => $header,
          '#rows' => $rows,
        ),
      );
    }
  }

  if (empty($build)) {
    return array(
      '#markup' => '<p>' . t('There is no content awaiting moderation at this time.'),
    );
  }

  return $build;
}
