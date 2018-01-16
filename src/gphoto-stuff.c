/* compile with gcc -Wall -o canon-capture -lgphoto2 canon-capture.c
 * This code released into the public domain 21 July 2008
 * 
 * This program does the equivalent of:
 * gphoto2 --shell
 *   > set-config capture=1
 *   > capture-image-and-download
 * compile with gcc -Wall -o canon-capture -lgphoto2 canon-capture.c
 *
 * Taken from: http://credentiality2.blogspot.com/2008/07/linux-libgphoto2-image-capture-from.html 
 */

#ifdef GPHOTO

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <gphoto2/gphoto2.h>

#include "impdefs.h"
#include "samples.h"
#include "gp-params.h"
#define _(String) (String)

#define ALL 0
#define VALUE 1

int gphoto_inited = 0;
static Camera	*camera;
static GPContext *cameracontext;
static GPParams gp_params;


/*
static void errordumper(GPLogLevel level, const char *domain, const char *format,
                 va_list args, void *data) {
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
}
*/

static int
_find_widget_by_name (GPParams *p, const char *name, CameraWidget **child, CameraWidget **rootconfig) {
	int	ret;
	
	ret = gp_camera_get_config (p->camera, rootconfig, p->context);
	if (ret != GP_OK) return ret;
	ret = gp_widget_get_child_by_name (*rootconfig, name, child);
	if (ret != GP_OK) 
		ret = gp_widget_get_child_by_label (*rootconfig, name, child);
	if (ret != GP_OK) {
		char		*part, *s, *newname;
		
		newname = strdup (name);
		if (!newname)
			return GP_ERROR_NO_MEMORY;
		
		*child = *rootconfig;
		part = newname;
		while (part[0] == '/')
			part++;
		while (1) {
			CameraWidget *tmp;
			
			s = strchr (part,'/');
			if (s)
				*s='\0';
			ret = gp_widget_get_child_by_name (*child, part, &tmp);
			if (ret != GP_OK)
				ret = gp_widget_get_child_by_label (*child, part, &tmp);
			if (ret != GP_OK)
				break;
			*child = tmp;
			if (!s) /* end of path */
				break;
			part = s+1;
			while (part[0] == '/')
				part++;
		}
		if (s) { /* if we have stuff left over, we failed */
			gp_context_error (p->context, _("%s not found in configuration tree."), newname);
			free (newname);
			gp_widget_free (*rootconfig);
			return GP_ERROR;
		}
		free (newname);
	}
	return GP_OK;
}

/* From the strftime(3) man page:
 * BUGS
 *     Some buggy versions of gcc complain about the use of %c: warning:
 *     %c yields only last 2 digits of year in some  locales.
 *     Of course programmers are encouraged to use %c, it gives the
 *     preferred date and time representation. One meets all kinds of
 *     strange obfuscations to circumvent this gcc problem. A relatively
 *     clean one is to add an intermediate function
 */

static size_t
my_strftime(char *s, size_t max, const char *fmt, const struct tm *tm)
{
	return strftime(s, max, fmt, tm);
}



int
get_config_action (GPParams *p, const char *name, int do_print) {
	CameraWidget *rootconfig,*child;
	int	ret;
	const char *label;
	CameraWidgetType	type;
	
	ret = _find_widget_by_name (p, name, &child, &rootconfig);
	if (ret != GP_OK)
		return ret;
	
	ret = gp_widget_get_type (child, &type);
	if (ret != GP_OK) {
		gp_widget_free (rootconfig);
		return ret;
	}
	ret = gp_widget_get_label (child, &label);
	if (ret != GP_OK) {
		gp_widget_free (rootconfig);
		return ret;
	}
	
	if(do_print == ALL) printf ("Label: %s\n", label); /* "Label:" is not i18ned, the "label" variable is */
	switch (type) {
		case GP_WIDGET_TEXT: {		/* char *		*/
			char *txt;
			
			ret = gp_widget_get_value (child, &txt);
			if (ret == GP_OK) {
				if(do_print == ALL) printf ("Type: TEXT\n"); /* parsed by scripts, no i18n */
				printf ("Current: %s\n",txt);
			} else {
				gp_context_error (p->context, _("Failed to retrieve value of text widget %s."), name);
			}
			break;
		}
		case GP_WIDGET_RANGE: {	/* float		*/
			float	f, t,b,s;
			
			ret = gp_widget_get_range (child, &b, &t, &s);
			if (ret == GP_OK)
				ret = gp_widget_get_value (child, &f);
			if (ret == GP_OK) {
				if(do_print == ALL) printf ("Type: RANGE\n");	/* parsed by scripts, no i18n */
				printf ("Current: %g\n", f);	/* parsed by scripts, no i18n */
				if(do_print == ALL) printf ("Bottom: %g\n", b);	/* parsed by scripts, no i18n */
				if(do_print == ALL) printf ("Top: %g\n", t);	/* parsed by scripts, no i18n */
				if(do_print == ALL) printf ("Step: %g\n", s);	/* parsed by scripts, no i18n */
			} else {
				gp_context_error (p->context, _("Failed to retrieve values of range widget %s."), name);
			}
			break;
		}
		case GP_WIDGET_TOGGLE: {	/* int		*/
			int	t;
			
			ret = gp_widget_get_value (child, &t);
			if (ret == GP_OK) {
				if(do_print == ALL) printf ("Type: TOGGLE\n");
				printf ("Current: %d\n",t);
			} else {
				gp_context_error (p->context, _("Failed to retrieve values of toggle widget %s."), name);
			}
			break;
		}
		case GP_WIDGET_DATE:  {		/* int			*/
			int	ret, t;
			time_t	xtime;
			struct tm *xtm;
			char	timebuf[200];
			
			ret = gp_widget_get_value (child, &t);
			if (ret != GP_OK) {
				gp_context_error (p->context, _("Failed to retrieve values of date/time widget %s."), name);
				break;
			}
			xtime = t;
			xtm = localtime (&xtime);
			ret = my_strftime (timebuf, sizeof(timebuf), "%c", xtm);
			if(do_print == ALL) printf ("Type: DATE\n");
			if(do_print == ALL) printf ("Current: %d\n", t);
			printf ("Printable: %s\n", timebuf);
			break;
		}
		case GP_WIDGET_MENU:
		case GP_WIDGET_RADIO: { /* char *		*/
			int cnt, i;
			char *current;
			
			ret = gp_widget_get_value (child, &current);
			if (ret == GP_OK) {
				cnt = gp_widget_count_choices (child);
				if (type == GP_WIDGET_MENU)
					if(do_print == ALL) printf ("Type: MENU\n");
				else
					if(do_print == ALL) printf ("Type: RADIO\n");
				printf ("Current: %s\n",current);
				for ( i=0; i<cnt; i++) {
					const char *choice;
					ret = gp_widget_get_choice (child, i, &choice);
					if(do_print == ALL) printf ("Choice: %d %s\n", i, choice);
				}
			} else {
				gp_context_error (p->context, _("Failed to retrieve values of radio widget %s."), name);
			}
			break;
		}
			
			/* ignore: */
		case GP_WIDGET_WINDOW:
		case GP_WIDGET_SECTION:
		case GP_WIDGET_BUTTON:
			break;
	}
	gp_widget_free (rootconfig);
	return (GP_OK);
}

int
set_config_action (GPParams *p, const char *name, const char *value) {
	CameraWidget *rootconfig,*child;
	int	ret;
	const char *label;
	CameraWidgetType	type;
	
	ret = _find_widget_by_name (p, name, &child, &rootconfig);
	if (ret != GP_OK)
		return ret;
	
	ret = gp_widget_get_type (child, &type);
	if (ret != GP_OK) {
		gp_widget_free (rootconfig);
		return ret;
	}
	ret = gp_widget_get_label (child, &label);
	if (ret != GP_OK) {
		gp_widget_free (rootconfig);
		return ret;
	}
	
	switch (type) {
		case GP_WIDGET_TEXT: {		/* char *		*/
			ret = gp_widget_set_value (child, value);
			if (ret != GP_OK)
				gp_context_error (p->context, _("Failed to set the value of text widget %s to %s."), name, value);
			break;
		}
		case GP_WIDGET_RANGE: {	/* float		*/
			float	f,t,b,s;
			
			ret = gp_widget_get_range (child, &b, &t, &s);
			if (ret != GP_OK)
				break;
			if (!sscanf (value, "%f", &f)) {
				gp_context_error (p->context, _("The passed value %s is not a floating point value."), value);
				ret = GP_ERROR_BAD_PARAMETERS;
				break;
			}
			if ((f < b) || (f > t)) {
				gp_context_error (p->context, _("The passed value %f is not within the expected range %f - %f."), f, b, t);
				ret = GP_ERROR_BAD_PARAMETERS;
				break;
			}
			ret = gp_widget_set_value (child, &f);
			if (ret != GP_OK)
				gp_context_error (p->context, _("Failed to set the value of range widget %s to %f."), name, f);
			break;
		}
		case GP_WIDGET_TOGGLE: {	/* int		*/
			int	t;
			
			t = 2;
			if (	!strcasecmp (value, "off")	|| !strcasecmp (value, "no")	||
				!strcasecmp (value, "false")	|| !strcmp (value, "0")		||
				!strcasecmp (value, _("off"))	|| !strcasecmp (value, _("no"))	||
				!strcasecmp (value, _("false"))
				)
				t = 0;
			if (	!strcasecmp (value, "on")	|| !strcasecmp (value, "yes")	||
				!strcasecmp (value, "true")	|| !strcmp (value, "1")		||
				!strcasecmp (value, _("on"))	|| !strcasecmp (value, _("yes"))	||
				!strcasecmp (value, _("true"))
				)
				t = 1;
			/*fprintf (stderr," value %s, t %d\n", value, t);*/
			if (t == 2) {
				gp_context_error (p->context, _("The passed value %s is not a valid toggle value."), value);
				ret = GP_ERROR_BAD_PARAMETERS;
				break;
			}
			ret = gp_widget_set_value (child, &t);
			if (ret != GP_OK)
				gp_context_error (p->context, _("Failed to set values %s of toggle widget %s."), value, name);
			break;
		}
		case GP_WIDGET_DATE:  {		/* int			*/
			int	t = -1;
			//struct tm xtm;
			
#ifdef HAVE_STRPTIME
			if (strptime (value, "%c", &xtm) || strptime (value, "%Ec", &xtm))
				t = mktime (&xtm);
#endif
			if (t == -1) {
				if (!sscanf (value, "%d", &t)) {
					gp_context_error (p->context, _("The passed value %s is neither a valid time nor an integer."), value);
					ret = GP_ERROR_BAD_PARAMETERS;
					break;
				}
			}
			ret = gp_widget_set_value (child, &t);
			if (ret != GP_OK)
				gp_context_error (p->context, _("Failed to set new time of date/time widget %s to %s."), name, value);
			break;
		}
		case GP_WIDGET_MENU:
		case GP_WIDGET_RADIO: { /* char *		*/
			int cnt, i;
			
			cnt = gp_widget_count_choices (child);
			if (cnt < GP_OK) {
				ret = cnt;
				break;
			}
			ret = GP_ERROR_BAD_PARAMETERS;
			for ( i=0; i<cnt; i++) {
				const char *choice;
				
				ret = gp_widget_get_choice (child, i, &choice);
				if (ret != GP_OK)
					continue;
				if (!strcmp (choice, value)) {
					ret = gp_widget_set_value (child, value);
					break;
				}
			}
			if (i != cnt)
				break;
			
			if (sscanf (value, "%d", &i)) {
				if ((i>= 0) && (i < cnt)) {
					const char *choice;
					
					ret = gp_widget_get_choice (child, i, &choice);
					if (ret == GP_OK)
						ret = gp_widget_set_value (child, choice);
					break;
				}
			}
			gp_context_error (p->context, _("Choice %s not found within list of choices."), value);
			break;
		}
			
			/* ignore: */
		case GP_WIDGET_WINDOW:
		case GP_WIDGET_SECTION:
		case GP_WIDGET_BUTTON:
			gp_context_error (p->context, _("The %s widget is not configurable."), name);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
	}
	if (ret == GP_OK) {
		ret = gp_camera_set_config (p->camera, rootconfig, p->context);
		if (ret != GP_OK)
			gp_context_error (p->context, _("Failed to set new configuration value %s for configuration entry %s."), value, name);
	}
	gp_widget_free (rootconfig);
	return (ret);
}



static void
capture_to_file(Camera *camera, GPContext *cameracontext, char *fn) {
	int fd, retval;
	CameraFile *camerafile;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

	/* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, cameracontext);
	if(retval != GP_OK) printf("  capture return: %d\n", retval);
    
	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	fd = open(fn, O_CREAT | O_WRONLY, 0644);
	retval = gp_file_new_from_fd(&camerafile, fd);
	if(retval != GP_OK) printf("  new file from fd return: %d\n", retval);
	retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
		     GP_FILE_TYPE_NORMAL, camerafile, cameracontext);
	if(retval != GP_OK) printf("  get camera file return: %d\n", retval);

	//printf("Deleting.\n");
	retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,
			cameracontext);
	if(retval != GP_OK) printf("  camera file delete return: %d\n", retval);

	gp_file_free(camerafile);
}

static void
display_widgets (CameraWidget *widget, char *prefix) {
	int 	ret, n, i;
	char	*newprefix;
	const char *label, *name, *uselabel;
	CameraWidgetType	type;
	
	gp_widget_get_label (widget, &label);
	/* fprintf(stderr,"label is %s\n", label); */
	ret = gp_widget_get_name (widget, &name);
	/* fprintf(stderr,"name is %s\n", name); */
	gp_widget_get_type (widget, &type);
	
	if (strlen(name))
		uselabel = name;
	else
		uselabel = label;
	
	n = gp_widget_count_children (widget);
	
	newprefix = malloc(strlen(prefix)+1+strlen(uselabel)+1);
	if (!newprefix)
		abort();
	sprintf(newprefix,"%s/%s",prefix,uselabel);
	
	if ((type != GP_WIDGET_WINDOW) && (type != GP_WIDGET_SECTION)) {
		printf("%s\n",newprefix);
		for(i=strlen(newprefix)-1; newprefix[i] != '/' && i!= 0; i--);
		//printf("\t%s\n",&newprefix[i+1]);
		printf("\t");
		ret = get_config_action (&gp_params, &newprefix[i+1],VALUE );
	}
	for (i=0; i<n; i++) {
		CameraWidget *child;
		
		ret = gp_widget_get_child (widget, i, &child);
		if (ret != GP_OK)
			continue;
		display_widgets (child, newprefix);
	}
	free(newprefix);
}


int init_gphoto_cam()
{
	int	retval;
	
	// if this process is running, we won't get the camera, so kill it
	system("killall PTPCamera");
	
	cameracontext = sample_create_context();
	gp_params.context = cameracontext;
	// mbl this not found
	//gp_log_add_func(GP_LOG_ERROR, errordumper, NULL);
	
	gp_camera_new(&camera);
	gp_params.camera = camera;
	
	/* When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
	 * init function seems to traverse the entire filesystem on the camera.  This
	 * is partly why it takes so long.
	 * (Marcus: the ptp2 driver does this by default currently.)
	 */
	printf("Camera init.  Takes about 10 seconds.\n");
	retval = gp_camera_init(camera, cameracontext);
	if (retval != GP_OK) {
		printf("  Retval: %d\n", retval);
		return (-1);
	}
	gphoto_inited = 1;
	return GP_OK;
}

int close_gphoto_cam()
{
	
	if(gphoto_inited) gp_camera_exit(camera, cameracontext);
	gphoto_inited = 0;
	
	return 0;
}

int capture(int n, int index)
{

	//int	retval;
	extern char     cmnd[],*fullname();
	
	if(!gphoto_inited) {
		if(init_gphoto_cam() != GP_OK){
			beep();
			printf("Can't init camera.\n");
		}
	}
	
	canon_enable_capture(camera, TRUE, cameracontext);
	/*set_capturetarget(canon, canoncontext);*/
	capture_to_file(camera, cameracontext, fullname(&cmnd[index],SAVE_DATA_NO_SUFFIX));
	//gp_camera_exit(canon, canoncontext);
	return 0;
}

/*
 
 CAMLISTSETTINGS
 
 */

int camlistsettings(int n, int index)
{
	int	ret;
	CameraWidget *rootconfig;
	
	
	if(!gphoto_inited) {
		if(init_gphoto_cam() != GP_OK){
			beep();
			printf("Can't init camera.\n");
		}
	}

	ret = gp_camera_get_config (camera, &rootconfig, cameracontext);
	if (ret != GP_OK) return ret;
	display_widgets (rootconfig, "");
	gp_widget_free (rootconfig);
	
	return (GP_OK);
}

/*
 
 CAMEXPOSE frames
 
 This does not work reliably for reasons unknown
 It behaves like the commandline gphoto2 where exposures are made, but not read to the computer and deleted -- 
 after a while, an error of -1 is returned.
 
 Taken out of comdecx for now.
 
 To do bracketing, write a macero using capture and setting exposures through the macro.
 
 */

int camexpose(int n, int index)
{
    
    int	retval = GP_OK,i,fd;
    extern char     cmnd[],*fullname();
    CameraFilePath camera_file_path[100];
	CameraFile *camerafile;
	char fn[1024];
	
	if (n < 1) n = 1;	// the number of exposures to take

    if(!gphoto_inited) {
		if(init_gphoto_cam() != GP_OK){
			beep();
			printf("Can't init camera.\n");
		}
    }
    
    canon_enable_capture(camera, TRUE, cameracontext);
    
    printf("Capturing.\n");
    
    for(i=0; i<n && retval == GP_OK; i++){
		/* NOP: This gets overridden in the library to /capt0000.jpg */
		strcpy(camera_file_path[i].folder, "/");
		strcpy(camera_file_path[i].name, "foo.jpg");
		
		retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path[i], cameracontext);
		if(retval != GP_OK) printf("  capture return: %d\n", retval);
		
		printf("Pathname on the camera: %s/%s\n", camera_file_path[i].folder, camera_file_path[i].name);
    }

	for(i=0; i<n && retval == GP_OK; i++){
		sprintf(fn,"pic_%d.nef",i);
		sprintf(fn,fullname(fn,SAVE_DATA_NO_SUFFIX));
		printf("File is: %s\n",fn);
		fd = open(fn, O_CREAT | O_WRONLY, 0644);
		retval = gp_file_new_from_fd(&camerafile, fd);
		if(retval != GP_OK) printf("  new file from fd return: %d\n", retval);
		retval = gp_camera_file_get(camera, camera_file_path[i].folder, camera_file_path[i].name,
									GP_FILE_TYPE_NORMAL, camerafile, cameracontext);
		if(retval != GP_OK) printf("  get camera file return: %d\n", retval);
		
		//printf("Deleting.\n");
		retval = gp_camera_file_delete(camera, camera_file_path[i].folder, camera_file_path[i].name,
									   cameracontext);
		if(retval != GP_OK) printf("  camera file delete return: %d\n", retval);
		
		gp_file_free(camerafile);
    }
	
    return 0;
}

/*
 
 CAMGETSETTING setting_name
 
 */

int camgetsetting(int n, int index)
{
	int ret;
	extern char     cmnd[];
	
	if(!gphoto_inited) {
		if(init_gphoto_cam() != GP_OK){
			beep();
			printf("Can't init camera.\n");
		}
	}
	
	ret = get_config_action (&gp_params, &cmnd[index],ALL);
	if (ret != GP_OK) printf("  Error: %d\n", ret);

	return ret;
}

/*
 
 CAMSETSETTING setting_name setting_value
 
*/

int camsetsetting(int n, int index)
{
	int ret,i;
	extern char     cmnd[];
	
	if(!gphoto_inited) {
		if(init_gphoto_cam() != GP_OK){
			beep();
			printf("Can't init camera.\n");
		}
	}
	for(i=0; cmnd[index+i] != ' ' && cmnd[index+i] != 0; i++);
	cmnd[index+i] = 0;
	
	printf("Set %s to value: %s\n",&cmnd[index], &cmnd[index+i+1]);
	
	ret = set_config_action (&gp_params, &cmnd[index], &cmnd[index+i+1]);
	if (ret != GP_OK) {
		printf("  Error: %d\n", ret);
	}
	
	return ret;
}

#endif