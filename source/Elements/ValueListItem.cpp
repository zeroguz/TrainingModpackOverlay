#include <valueListItem.h>

ValueListItem::ValueListItem(std::string text, const std::vector<std::string> values, int defaultPos, const std::string data) : tsl::elm::ListItem(text), m_values(values), m_curValue(defaultPos), extdata(data)
{
}

ValueListItem::~ValueListItem()
{
}

tsl::elm::Element *ValueListItem::requestFocus(Element *oldFocus, tsl::FocusDirection direction)
{
  return ListItem::requestFocus(oldFocus, direction);
}

void ValueListItem::layout(u16 parentX, u16 parentY, u16 parentWidth, u16 parentHeight)
{
  ListItem::layout(parentX, parentY, parentWidth, parentHeight);
}

bool ValueListItem::onClick(u64 keys)
{
  if (keys & KEY_A)
  {
    m_curValue++;
    long int size = m_values.size();
    if (m_curValue < 0)
      m_curValue = size - 1;
    if (size <= m_curValue)
      m_curValue = 0;

    if (this->m_valueChangeListener != nullptr)
      this->m_valueChangeListener(this->m_values, this->m_curValue, this->extdata);

    return true;
  }

  return false;
}