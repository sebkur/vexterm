#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "xml_document.h"

int main(int argc, char * argv[])
{
	printf("testing the xml writer\n");

	g_thread_init(NULL);
	g_type_init();

	XmlDocument * doc = xml_document_new();
	XmlNode * root = xml_node_new("config");
	xml_document_set_root(doc, root);

	XmlNode * foo = xml_node_new("foo");
	xml_node_add_child(root, foo);
	xml_node_add_tag(foo, "a", "b");
	xml_node_add_tag(foo, "blah", "foo");

	foo = xml_node_new("asdf");
	xml_node_add_child(root, foo);
	xml_node_add_tag(foo, "c", "c");
	xml_node_add_tag(foo, "blah", "foo");

	XmlNode * bar = xml_node_new("bar");
	xml_node_add_child(foo, bar);
	xml_node_add_tag(bar, "x", "xasdfy");
	xml_node_add_tag(bar, "234", "sdflkj");

	int fd = open("foo.xml", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd > 0){
		xml_document_write(doc, fd);
		close(fd);
	}

	return 0;
}
