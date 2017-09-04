#include "visual.hpp"
#include "utils.hpp"
#include <unordered_map>

namespace Chess::Visual {

char const* Colors_not_supported::what() const noexcept
{
  return "Required colors are not supported on this terminal.";
}

Scoped_curses::Scoped_curses()
{
  // TODO Maybe these can be split into more functions
  initscr();

  if (!has_colors())
    throw Colors_not_supported();

  start_color();

  raw();
  noecho();
  keypad(stdscr, true);
  curs_set(0);
}

Scoped_curses::~Scoped_curses()
{
  endwin();
}

bool operator==(Color_pair const lhs, Color_pair const rhs)
{
  return lhs.foreground == rhs.foreground && 
         lhs.background == rhs.background;
}

Color Colors::color_for_pair(Color_pair const color_pair)
{
  if (!is_inited(color_pair))
    init_color_pair(color_pair);

  return COLOR_PAIR(color_ids_[color_pair]);
}

bool Colors::is_inited(Color_pair const color_pair) const
{
  return color_ids_.count(color_pair);
}

void Colors::init_color_pair(Color_pair const color_pair)
{
  color_ids_[color_pair] = new_color_id_;
  init_pair(new_color_id_, color_pair.foreground, color_pair.background);
  ++new_color_id_;
}

Board_view::Board_view(
  Game::Board const& board, 
  Game::Field_selection const& field_selection)
  : board_(&board)
  , field_selection_(&field_selection) 
{}

void Board_view::display()
{
  draw_tiles();
}

void Board_view::draw_tiles()
{
  for_each_field(
    [this](Game::Board_position const pos)
    {
      draw_tile(translate_to_screen(pos));
    }
  );
}

void Board_view::draw_tile(Screen_position const upper_left_pos)
{
  auto const color = tile_color();
  auto const [x, y] = upper_left_pos;

  auto const draw_corners = [&] 
  {
    mvaddch_colored(y,   x,   ACS_PLUS, color);
    mvaddch_colored(y+2, x,   ACS_PLUS, color);
    mvaddch_colored(y,   x+4, ACS_PLUS, color);
    mvaddch_colored(y+2, x+4, ACS_PLUS, color);
  };

  auto const draw_frame = [&] 
  {
    mvaddch_colored(y+1, x,   ACS_VLINE, color);
    mvaddch_colored(y+1, x+4, ACS_VLINE, color);

    mvaddch_colored(y,   x+2, ACS_HLINE, color);
    mvaddch_colored(y,   x+3, ACS_HLINE, color);
    mvaddch_colored(y,   x+1, ACS_HLINE, color);
    mvaddch_colored(y+2, x+2, ACS_HLINE, color);
    mvaddch_colored(y+2, x+3, ACS_HLINE, color);
    mvaddch_colored(y+2, x+1, ACS_HLINE, color);
  };
  
  draw_corners();
  draw_frame();
}

Color Board_view::tile_color()
{
  return colors_.color_for_pair({COLOR_WHITE, COLOR_BLACK});
}

void Board_view::draw_pieces()
{
  /*for_each_field(
    [](Game::Board_position const pos)
    {
      if ()
    }
  )
  for (auto const piece : *board_)
    draw_piece(piece);*/
}

void Board_view::draw_piece(Game::Piece const)
{

}

bool Board_view::field_is_selected(Game::Board_position const pos) const 
{
  return field_selection_->position() == pos;
}

Game::Board_position Board_view::translate_to_board(
  Screen_position const upper_left_pos)
{
  return Game::Board_position{
    upper_left_pos.x/field_width,
    upper_left_pos.y/field_height
  };
}

Screen_position Board_view::translate_to_screen(
  Game::Board_position const board_pos)
{
  return Screen_position{
    board_pos.x * field_width, 
    board_pos.y * field_height
  };
}

char piece_character_representation(Game::Piece const piece)
{
  return utils::visit_variant<char>(piece.kind,
    [](Game::Piece_kinds::Pawn const)
    {
      return 'P';
    },
    [](Game::Piece_kinds::Bishop const)
    {
      return 'B';
    }
  );
}

Scoped_color::Scoped_color(Color const color)
  : color_(color)
{
  attron(color_);
}

Scoped_color::~Scoped_color()
{
  attroff(color_);
}

void mvaddch_colored(
  int const y,
  int const x,
  chtype const character, 
  Color const color)
{
  Scoped_color scoped_color(color);
  mvaddch(y, x, character);
}

}

