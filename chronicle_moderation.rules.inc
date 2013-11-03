<?php

/**
 * @file
 * Provides Rules integration.
 */

/**
 * Implements hook_rules_event_info().
 */
function chronicle_moderation_rules_event_info() {
  return array(
    'node_newrevision' => array(
      'label' => t('Content revision has been created'),
      'group' => t('Revisioning'),
      'variables' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('New revision'),
        ),
      ),
      'class' => 'RulesNodeEventHandler',
    ),
  );
}
