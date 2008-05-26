#!/bin/bash
# Sidux-Control-Center
# Fabian Wuertz
# License: GPL

DIALOG="dialog"



# we need ssft 
if [ -f /usr/bin/ssft.sh ]; then
	. /usr/bin/ssft.sh || exit 1
else
		echo "Please install the package \"ssft\"."
		exit 1
fi

if [ -n "${frontend}" ]; then
	case $frontend in
		text)  SSFT_FRONTEND="${frontend}";;
		dialog)  SSFT_FRONTEND="${frontend}";;
		kdialog)  SSFT_FRONTEND="${frontend}";;
		zenity) SSFT_FRONTEND="${frontend}";;
		*)  echo unknown frontend && exit 1;;
	esac
fi

if [ -z "$DISPLAY" ]; then
	[ -x /usr/bin/dialog ] && SSFT_FRONTEND=${SSFT_FRONTEND:-"dialog"} || \
		SSFT_FRONTEND=${SSFT_FRONTEND:-"text"}
else
	if [ -n "$DISPLAY" ] && [ ! -x /usr/bin/zenity ]; then
		if [ -x /usr/bin/dialog ]; then
			SSFT_FRONTEND="dialog"
		else
			SSFT_FRONTEND="text"
		fi
		DISPLAY=""
	else
		SSFT_FRONTEND=${SSFT_FRONTEND:-"zenity"}
	fi
fi




about()
{
	ssft_display_message "${Title}" "$(eval_gettext "(c) 2007 by Fabian Wuertz\nhttp://sidux.com")"
}




#-------------------------------------------------------------------------------

ssft_menu() {
  # MENU strings
  ssft_set_textdomain
  _l_PROMPT_STR="`gettext "Option number (0 cancels)"`"
  _l_OPTIONS_STR="`gettext "Options"`"
  ssft_reset_textdomain
  
  # Local variables
  _l_title="";
  _l_question="";
  _l_ret=255;
  _l_string="";
  _l_option=0;
  _l_numitems=0;
  _l_count=0;
  
  # Unset result
  unset SSFT_RESULT

  # Save default and unset SSFT_DEFAULT
  _l_default="$SSFT_DEFAULT";
  unset SSFT_DEFAULT

  # Check args
  if [ "$#" -lt 3 ]; then
    return $_l_ret;
  fi
  
  # Set arguments 
  _l_title="$1"; 
  _l_question="$2";
  shift 2;
  _l_numitems=$#;
  
  # Read values 
  case "$SSFT_FRONTEND" in
  zenity)
    _l_zitems="";
    _l_out="";
    for _l_item in "$@"; do
      if [ -z "$_l_zitems" ]; then
        _l_zitems="'$_l_item'"
      else
        _l_zitems="$_l_zitems '$_l_item'"
      fi
    done
    _l_string=$(echo "$_l_zitems" \
      | xargs zenity --title "$_l_title" --list --text "$_l_question" \
      --column "" 2> /dev/null)
  ;;
  kdialog)
    _l_zitems="";
    _l_out="";
    for _l_item in "$@"; do
      if [ "$_l_item" = "$_l_default" ]; then
        _l_selected="on";
      else
        _l_selected="off";
      fi
      if [ -z "$_l_zitems" ]; then
        _l_zitems="'$_l_item' '$_l_item' $_l_selected"
      else
        _l_zitems="$_l_zitems '$_l_item' '$_l_item' $_l_selected"
      fi
    done
    _l_out=$(echo "$_l_zitems" \
      | xargs kdialog --title "$_l_title" --radiolist "$_l_question" \
        2> /dev/null)
    _l_string=$(echo $_l_out | sed -e 's/^"//; s/"$//; s/" "/\n/g;');
  ;;
  dialog)
    _l_ditems="";
    for _l_item in "$@"; do
      if [ -z "$_l_ditems" ]; then
        _l_ditems="'$_l_item' ''"
      else
        _l_ditems="$_l_ditems '$_l_item' ''"
      fi
    done
    _l_out=$( echo "$_l_ditems" \
      | xargs dialog --stdout --title "$_l_title" \
      --menu "$_l_question" 0 0 5 2> /dev/null );
    _l_string=$(echo $_l_out | sed -e 's/^"//; s/"$//; s/" "/\n/g;');
  ;;
  text)
    ssft_print_text_title "$_l_title"
    while true; do
      _l_count=0;
      for _l_item in "$@"; do
	if [ "$_l_count" -eq "0" ]; then
          echo "$_l_question"
          echo ""
	fi
        _l_count=$(( $_l_count + 1 ))
	if [ "$_l_item" = "$_l_default" ]; then
	  _l_selected="*"
	else
	  _l_selected=" "
	fi
        printf "%s %2s. %s\n" "$_l_selected" "$_l_count" "$_l_item"
      done | more
      echo ""
      printf "%s: " "$_l_PROMPT_STR"
      read _l_option
      if [ -n "$_l_default" ] && [ "$_l_option" = "" ]; then
	_l_string="$_l_default"
	_l_ret=0
	break;
      fi
      _l_option=$(echo $_l_option \
        | sed -n -e '/^[[:space:]]*[0-9][0-9]*[[:space:]]*$/ {
          s/[^0-9]//g;
          p;
        };')
      if [ -n "$_l_option" ]; then
        if [ "$_l_option" -le "0" ]; then
          _l_ret=256
          break;
        elif [ "$_l_option" -le "$_l_numitems" ]; then
	  _l_ret=0
	  break;
	fi
      fi
    done
    echo ""
  ;;
  *)
    ssft_print_text_title "$_l_title"
    echo "$_l_question"
    _l_count=0;
    for _l_item in "$@"; do
      _l_count=$(( $_l_count + 1 ))
      if [ "$_l_item" = "$_l_default" ]; then
        _l_selected="*"
      else
        _l_selected=" "
      fi
      printf "%s %2s. %s\n" "$_l_selected" "$_l_count" "$_l_item"
    done
    echo ""
    if [ -n "$_l_default" ]; then
      echo "$_l_PROMPT_STR: ."
      _l_string="$_l_default"
      _l_ret=0
    else
      echo "$_l_PROMPT_STR: 0"
      _l_ret=1
    fi
  ;;
  esac
  # Get the string for the dialog and text frontends
  if [ "$_l_ret" = "0" ] && [ -z "$_l_string" ]; then
    _l_count=0;
    for _l_item in "$@"; do
      _l_count=$(( $_l_count + 1 ))
      if [ "$_l_option" = "$_l_count" ]; then
        _l_string="$_l_item";
	break;
      fi
    done
  fi
  SSFT_RESULT="$_l_string"
  test -n "$_l_string"
  return $?
}

