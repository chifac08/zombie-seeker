Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"
  config.vm.provision :shell, path: "vagrant/bootstrap.sh", run: 'always'
end
