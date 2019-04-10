<?php

/**
 * Class ThrobberTypes
 */
abstract class ThrobberTypes {

  const CHASING_DOTS = 'ThrobberChasingDots';
  const CIRCLE = 'ThrobberCircle';
  const CUBE_GRID = 'ThrobberCubeGrid';
  const DOUBLE_BOUNCE = 'ThrobberDoubleBounce';
  const FADING_CIRCLE = 'ThrobberFadingCircle';
  const FOLDING_CUBE = 'ThrobberFoldingCube';
  const PULSE = 'ThrobberPulse';
  const ROTATING_PLANE = 'ThrobberRotatingPlane';
  const THREE_BOUNCE = 'ThrobberThreeBounce';
  const WANDERING_CUBES = 'ThrobberWanderingCubes';
  const WAVE = 'ThrobberWave';


  /**
   * Returns array of all throbber types with their respective labels.
   * @return array
   */
  public static function toArray() {
    return array(
      self::CHASING_DOTS => 'Chasing dots',
      self::CIRCLE => 'Circle',
      self::CUBE_GRID => 'Cube grid',
      self::DOUBLE_BOUNCE => 'Double bounce',
      self::FADING_CIRCLE => 'Fading circle',
      self::FOLDING_CUBE => 'Folding cube',
      self::PULSE => 'Pulse',
      self::ROTATING_PLANE => 'Rotating plane',
      self::THREE_BOUNCE => 'Three bounce',
      self::WANDERING_CUBES => 'Wandering cubes',
      self::WAVE => 'Wave',
    );
  }

}