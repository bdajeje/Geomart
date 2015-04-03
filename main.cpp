#include <iostream>

#include <SFML/Graphics.hpp>

// All following units are in pixels
constexpr int WINDOW_WIDTH  = 600;
constexpr int WINDOW_HEIGHT = 600;
constexpr int CIRCLE_RADIUS = 10;
constexpr int CIRCLE_MARGIN = 30;

constexpr float MOVE       = CIRCLE_RADIUS * 2; // pixels
constexpr float MOVE_TIME  = 0.25; // seconds
constexpr float MOVE_SPEED = static_cast<float>(MOVE) / MOVE_TIME / 1000.0; // pixels/ms

enum class Direction
{
  LEFT, RIGHT,
  TOP_RIGHT, BOTTOM_LEFT
};

/*! The object structure, represents the Circles */
struct Object
{
  Object(const sf::Vector2f& position, const sf::Vector2f& destination)
    : shape {CIRCLE_RADIUS}
    , destination {destination}
  {
    shape.setFillColor(sf::Color(255, 255, 255));
    shape.setPosition( position.x, position.y );

    // Possible starting direction are only to the left or the right
    direction = ( position.x > destination.x ) ? Direction::LEFT : Direction::RIGHT;
  }

  sf::CircleShape shape;
  sf::Vector2f destination;
  Direction direction;
};

/*! Calculate the move to do in order to reach a destination by checking we don't go further the destiantion
 *  \param current_position - point where we actually are
 *  \param destination      - point to reach
 *  \param move_distance    - the speed, distance we are going to do
 *  \returns the given mov_distance parameter if current_position plus move_distance is not upper than destination.
 *           Otherwise returns destination.
 */
float calculateMoveDistance(float current_position, float destination, float move_distance)
{
  const auto distance = destination - current_position;

  if( distance < 0 )
    return ( current_position - move_distance < destination ) ? distance : -move_distance;
  else if( distance > 0 )
    return ( current_position + move_distance > destination ) ? distance : move_distance;

  return 0;
}

/*! Is a given number odd ? */
bool isOdd(int number)
{
  return (number & 1);
}

/*! Update position of an object then draw it */
void draw(sf::RenderWindow& window, Object& object, float pixel_move)
{
  // Move the shape
  object.shape.move( calculateMoveDistance( object.shape.getPosition().x, object.destination.x, pixel_move ),
                     calculateMoveDistance( object.shape.getPosition().y, object.destination.y, pixel_move ));

  // Draw object
  window.draw(object.shape);
}

/*! Update circles destination
 *  \param objects - to update
 *  \note went to left : next destination is to top right
 *        went to right : next destination is to bottom left
 */
void updateDestination(std::vector<Object>& objects)
{
  for( Object& object : objects )
  {
    const auto& current_position = object.shape.getPosition();

    switch(object.direction)
    {
      case Direction::LEFT:
      {
        object.destination = sf::Vector2f{ current_position.x + MOVE * 2, current_position.y - MOVE * 2 };
        object.direction = Direction::TOP_RIGHT;
        break;
      }
      case Direction::RIGHT:
      {
        object.destination = sf::Vector2f{ current_position.x - MOVE * 2, current_position.y + MOVE * 2 };
        object.direction = Direction::BOTTOM_LEFT;
        break;
      }
      case Direction::TOP_RIGHT:
      {
        object.destination = sf::Vector2f{ current_position.x + MOVE, current_position.y };
        object.direction = Direction::RIGHT;
        break;
      }
      case Direction::BOTTOM_LEFT:
      {
        object.destination = sf::Vector2f{ current_position.x - MOVE, current_position.y };
        object.direction = Direction::LEFT;
        break;
      }
    }
  }
}

int main()
{
  // Creating window
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Geomart 1");
  window.setFramerateLimit(30);

  // Build shapes
  std::vector<Object> objects;
  constexpr auto nbr_shapes_by_row = WINDOW_WIDTH / (CIRCLE_MARGIN + CIRCLE_RADIUS) + 1;
  constexpr auto nbr_shapes_by_col = WINDOW_HEIGHT / (CIRCLE_MARGIN + CIRCLE_RADIUS) + 1;
  constexpr auto nbr_shapes = nbr_shapes_by_row * nbr_shapes_by_col;
  constexpr auto vertical_margin = CIRCLE_MARGIN / 2;
  objects.reserve(nbr_shapes);
  for( size_t i = 0; i < nbr_shapes; ++i )
  {
    const bool is_odd_vectical_pos = isOdd(i / nbr_shapes_by_row);
    float x_offset = (!is_odd_vectical_pos) ? -MOVE / 2 : MOVE / 2;

    // Circle position
    sf::Vector2f position {static_cast<float>((CIRCLE_RADIUS + CIRCLE_MARGIN) * (i % nbr_shapes_by_row)) - x_offset,
                           static_cast<float>((CIRCLE_RADIUS + CIRCLE_MARGIN) * (i / nbr_shapes_by_row) + vertical_margin) };

    // Circle destination
    const auto direction = (is_odd_vectical_pos) ? -MOVE : MOVE;
    sf::Vector2f destination { position.x + direction, position.y };

    // Add circle
    objects.emplace_back( position, destination );
  }

  // Time
  sf::Clock clock;

  // Main loop
  while(window.isOpen())
  {
    // Handle events
    sf::Event event;
    while(window.pollEvent(event))
    {
      if(event.type == sf::Event::Closed)
        window.close();
    }

    // Clear entire screen
    window.clear();

    // Check if circles reached their destination
    // To do that, simply check one of them becuase same rule applies for all
    const Object& first_circle = objects.front();
    if( first_circle.destination.x == first_circle.shape.getPosition().x &&
        first_circle.destination.y == first_circle.shape.getPosition().y )
      updateDestination( objects );

    // Update shapes
    const auto pixel_move = MOVE_SPEED * clock.restart().asMilliseconds();
    for( Object& object : objects )
      draw( window, object, pixel_move );

    // Render screen
    window.display();
  }

  return EXIT_SUCCESS;
}

