BUCKET = s3://doc.adamh.cz/nanomidi/
SITE_DIR = html

.PHONY: doc
doc:
	doxygen

.PHONY: clean
clean:
	rm -rf html

.PHONY: distclean
distclean: clean

.PHONY: publish
publish: doc
	# Sync CSS and JS (CacheL expire in 1 week)
	s3cmd sync --exclude '*.*' --include '*.css' \
	--no-mime-magic \
	--add-header='Content-Type: text/css' \
	--add-header='Cache-Control: max-age=604800' \
	$(SITE_DIR)/ $(BUCKET)

	s3cmd sync --exclude '*.*' --include '*.js' \
	--no-mime-magic \
	--add-header='Content-Type: application/javascript' \
	--add-header='Cache-Control: max-age=604800' \
	$(SITE_DIR)/ $(BUCKET)

	# Sync media files (Cache: expire in 10 weeks)
	s3cmd sync --exclude '*.*' --include '*.png' --include '*.jpg' \
	--no-mime-magic \
	--include '*.ico' --add-header='Expires: Sat, 20 Nov 2020 18:46:39 GMT' \
	--add-header='Cache-Control: max-age=6048000' $(SITE_DIR)/ $(BUCKET)

	# Sync html files (Cache: 2 hours)
	s3cmd sync --exclude '*.*' --include '*.html' \
	--no-mime-magic \
	--add-header='Content-Type: text/html' \
	--add-header='Cache-Control: max-age=7200, must-revalidate' \
	$(SITE_DIR)/ $(BUCKET)

	# Sync everything else
	s3cmd sync --no-mime-magic --delete-removed $(SITE_DIR)/ $(BUCKET)
