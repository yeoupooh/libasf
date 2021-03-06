
#include <stdio.h>
#include "asf.h"

static void
print_metadata(asf_metadata_t *metadata) {
	int i;

	printf("----- PRINT METADATA -----\n");
	if (metadata->title)
		printf("title: %s\n", metadata->title);
	if (metadata->artist)
		printf("artist: %s\n", metadata->artist);
	if (metadata->copyright)
		printf("copyright: %s\n", metadata->copyright);
	if (metadata->description)
		printf("description: %s\n", metadata->description);
	if (metadata->rating)
		printf("rating: %s\n", metadata->rating);
	for (i=0; i<metadata->extended_count; i++) {
		printf("\"%s\" - \"%s\"\n", metadata->extended[i].key, metadata->extended[i].value);
	}
	printf("-----  END METADATA -----\n");
}

int main(int argc, char *argv[]) {
	asf_file_t *file;
	asf_metadata_t *metadata;
	asf_stream_t *stream;
	asf_packet_t *pkt;
	int i;

	if (argc != 2) {
		printf("Incorrect number of arguments\n");
		return -1;
	}

	file = asf_open_file(argv[1]);
	if (!file) {
		printf("Error opening file %s\n", argv[1]);
		return -1;
	}

	asf_init(file);
	metadata = asf_header_get_metadata(file);
	if (metadata) {
		print_metadata(metadata);
		asf_metadata_destroy(metadata);
	}

	stream = asf_get_stream(file, 1);
	printf("Stream type: %d\n", stream->type);
	if (stream->type == ASF_STREAM_TYPE_AUDIO) {
		asf_waveformatex_t *wav = stream->properties;

		printf("Audio format found:\n");
		for (i=0; i<wav->cbSize; i++) {
			printf("%02x", wav->data[i]);
		}
		printf("\n");
	}

	stream = asf_get_stream(file, 2);
	if (stream->type == ASF_STREAM_TYPE_VIDEO) {
		asf_bitmapinfoheader_t *bmp = stream->properties;

		printf("Video format found: %04x\n", bmp->biSize);
		for (i=0; i<bmp->biSize-ASF_BITMAPINFOHEADER_SIZE; i++) {
			printf("%02x", bmp->data[i]);
		}
		printf("\n");
	}

	pkt = asf_packet_create();
	for (i=0; i<50; i++) {
		int tmp;

		if ((tmp = asf_seek_to_msec(file, (uint64_t) i*1000)) < 0) {
			printf("Seek failed\n");
		}

		if ((tmp = asf_get_packet(file, pkt)) < 0) {
			printf("Error %d getting packet\n", tmp);
			break;
		}

		if (!tmp) {
			printf("EOF found\n");
			break;
		}
	}
	asf_packet_destroy(pkt);
/*
	printf("position after seek %lld\n", 
	asf_seek_to_msec(file, 18000));
*/
	asf_close(file);

	return 0;
}
