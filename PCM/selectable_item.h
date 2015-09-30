#ifndef _SELECTABLE_ITEM_H
#define _SELECTABLE_ITEM_H

#include "basic_types.h"
#include "color_table.h"
class SelectableItem 
{
public:
	SelectableItem( const ColorType& c = ColorType(0.,0.,0.,1.) ):color_(c), 
		visible_(true), 
		selected_(false),
		hightlighted_(false){}

	virtual ~SelectableItem(){}

	virtual void set_color( const ColorType& c ){ color_ = c; }
	virtual const ColorType  color()
	{
		if (selected_)
 			return SELECTED_COLOR;
		else if( hightlighted_ )
			return HIGHTLIGHTED_COLOR;
		else
			return color_;
	}

	virtual bool is_visible() const { return visible_; }
	virtual void set_visble(const bool v) { visible_ = v; }
	virtual bool is_selected() const { return selected_; }
	virtual void set_selected( const bool s ){ selected_ = s; }
	virtual bool is_hightlighted() const { return hightlighted_; }
	virtual void set_hightlighted( const bool h ){ hightlighted_ = h; }



protected:
	bool visible_;
	bool selected_;
	bool hightlighted_;
	ColorType color_;


};

#endif